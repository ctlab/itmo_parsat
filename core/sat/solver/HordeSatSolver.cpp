#include "core/sat/solver/HordeSatSolver.h"

namespace {

constexpr int SOLVING_DONE = -10;

void solverRunningThread(
    PortfolioSolverInterface* solver, Mutex& interruptLock, bool& solvingDoneLocal,
    core::sat::State& result, std::vector<int> const& assumptions) {
  while (true) {
    interruptLock.lock();
    if (solvingDoneLocal) {
      interruptLock.unlock();
      break;
    } else {
      solver->unsetSolverInterrupt();
    }
    interruptLock.unlock();
    SatResult res = solver->solve(assumptions);
    if (res == SAT) {
      solvingDoneLocal = true;
      result = core::sat::SAT;
    }
    if (res == UNSAT) {
      solvingDoneLocal = true;
      result = core::sat::UNSAT;
    }
  }
}

void stopAllSolvers(
    Mutex& interruptLock, bool& solvingDoneLocal, std::vector<PortfolioSolverInterface*>& solvers) {
  int solversCount = (int) solvers.size();
  interruptLock.lock();
  solvingDoneLocal = true;
  for (int i = 0; i < solversCount; i++) {
    solvers[i]->setSolverInterrupt();
  }
  interruptLock.unlock();
}

bool getGlobalEnding(
    int mpi_size, int mpi_rank, bool& solvingDoneLocal, int* endingBuffer, Mutex& interruptLock,
    std::vector<PortfolioSolverInterface*>& solvers) {
  int sendMsg = 0;
  if (solvingDoneLocal) {
    sendMsg = SOLVING_DONE;
  }
  MPI_Allgather(&sendMsg, 1, MPI_INT, endingBuffer, 1, MPI_INT, MPI_COMM_WORLD);
  for (int r = 0; r < mpi_size; r++) {
    if (endingBuffer[r] == SOLVING_DONE) {
      stopAllSolvers(interruptLock, solvingDoneLocal, solvers);
      return true;
    }
  }
  return false;
}

void sparseDiversification(
    int mpi_size, int mpi_rank, std::vector<PortfolioSolverInterface*>& solvers) {
  int solversCount = (int) solvers.size();
  int totalSolvers = mpi_size * solversCount;
  int vars = solvers[0]->getVariablesCount();
  for (int sid = 0; sid < solversCount; sid++) {
    int shift = (mpi_rank * solversCount) + sid;
    for (int var = 1; var + totalSolvers < vars; var += totalSolvers) {
      solvers[sid]->setPhase(var + shift, true);
    }
  }
}

void randomDiversification(unsigned int seed, std::vector<PortfolioSolverInterface*>& solvers) {
  int solversCount = (int) solvers.size();
  srand(seed);
  int vars = solvers[0]->getVariablesCount();
  for (int sid = 0; sid < solversCount; sid++) {
    for (int var = 1; var <= vars; var++) {
      solvers[sid]->setPhase(var, rand() % 2 == 1);
    }
  }
}

void sparseRandomDiversification(
    unsigned int seed, int mpi_size, std::vector<PortfolioSolverInterface*>& solvers) {
  int solversCount = (int) solvers.size();
  srand(seed);
  int totalSolvers = solversCount * mpi_size;
  int vars = solvers[0]->getVariablesCount();
  for (int sid = 0; sid < solversCount; sid++) {
    for (int var = 1; var <= vars; var++) {
      if (rand() % totalSolvers == 0) {
        solvers[sid]->setPhase(var, rand() % 2 == 1);
      }
    }
  }
}

void nativeDiversification(
    int mpi_rank, int mpi_size, std::vector<PortfolioSolverInterface*>& solvers) {
  int solversCount = (int) solvers.size();
  int base = mpi_rank * solversCount;
  for (int sid = 0; sid < solversCount; sid++) {
    solvers[sid]->diversify(sid + base, mpi_size * solversCount);
  }
}

void binValueDiversification(
    int mpi_size, int mpi_rank, std::vector<PortfolioSolverInterface*>& solvers) {
  int solversCount = (int) solvers.size();
  int totalSolvers = mpi_size * solversCount;
  int tmp = totalSolvers;
  int log = 0;
  while (tmp) {
    tmp >>= 1;
    log++;
  }
  int vars = solvers[0]->getVariablesCount();
  for (int sid = 0; sid < solversCount; sid++) {
    int num = mpi_rank * sid;
    for (int var = 1; var < vars; var++) {
      int bit = var % log;
      bool phase = (num >> bit) & 1 ? true : false;
      solvers[sid]->setPhase(var, phase);
    }
  }
}

}  // namespace

namespace core::sat {

HordeSatSolver::HordeSatSolver(HordeSatSolverConfig const& config)
    : _num_solvers(config.threads()), _cfg(config) {
  setVerbosityLevel(config.verbose());
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

  for (uint32_t i = 0; i < _num_solvers; ++i) {
    switch (config.seq_solver()) {
      case HordeSatSolverConfig_SeqSolver_MINISAT:
        _solvers.push_back(std::make_unique<MiniSat>());
        break;
      case HordeSatSolverConfig_SeqSolver_LINGELING:
        _solvers.push_back(std::make_unique<Lingeling>());
        break;
      case HordeSatSolverConfig_SeqSolver_COMBO:
        if ((mpi_rank + i) % 2 == 0) {
          _solvers.push_back(std::make_unique<MiniSat>());
        } else {
          _solvers.push_back(std::make_unique<Lingeling>());
        }
        break;
      default:
        IPS_TERMINATE("HordeSat: invalid seq_solver");
    }
    _solver_ptrs.push_back(_solvers.back().get());
  }

  switch (config.clause_exchange_mode()) {
    case HordeSatSolverConfig_ClauseExchangeMode_CEM_NONE:
      break;
    case HordeSatSolverConfig_ClauseExchangeMode_ALL_TO_ALL:
      _sharing_manager = std::make_unique<AllToAllSharingManager>(
          mpi_size, mpi_rank, _solver_ptrs, config.filter_duplicate_clauses());
      break;
    case HordeSatSolverConfig_ClauseExchangeMode_LOG_PARTNERS:
      _sharing_manager =
          std::make_unique<LogSharingManager>(mpi_size, mpi_rank, _solver_ptrs, false);
      break;
    default:
      IPS_TERMINATE("HordeSat: invalid clause exchange mode.");
  }

  // initialize ending detection
  _ending_buffer.reset(new int[mpi_size]);

  for (uint32_t tid = 0; tid < _num_solvers; ++tid) {
    _solver_threads.emplace_back([this, tid] {
      auto* solver = _solver_ptrs[tid];
      uint64_t local_solve_index = 0;

      while (!_stop) {
        std::unique_lock<std::mutex> ul(_slv_mutex);
        _slv_cv.wait(
            ul, [this, local_solve_index] { return _stop || _solve_idx != local_solve_index; });
        if (IPS_UNLIKELY(_stop)) {
          break;
        }
        if (IPS_UNLIKELY(local_solve_index == _solve_idx)) {
          // spurious wakeup
          continue;
        }
        local_solve_index = _solve_idx;
        ul.unlock();
        solverRunningThread(solver, _interruptLock, _solvingDoneLocal, _result, _assumptions);
      }
    });
  }
}

HordeSatSolver::~HordeSatSolver() noexcept {
  {
    std::lock_guard<std::mutex> lg(_slv_mutex);
    _stop = true;
  }
  _slv_cv.notify_all();
  for (auto& thread : _solver_threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}

void HordeSatSolver::_diversify() {
  switch (_cfg.diversification()) {
    case HordeSatSolverConfig_Diversification_DVS_NONE:
      break;
    case HordeSatSolverConfig_Diversification_SPARSE:
      sparseDiversification(mpi_size, mpi_rank, _solver_ptrs);
      break;
    case HordeSatSolverConfig_Diversification_RANDOM:
      randomDiversification(_cfg.seed(), _solver_ptrs);
      break;
    case HordeSatSolverConfig_Diversification_NATIVE:
      nativeDiversification(mpi_rank, mpi_size, _solver_ptrs);
      break;
    case HordeSatSolverConfig_Diversification_BIN:
      binValueDiversification(mpi_size, mpi_rank, _solver_ptrs);
      break;
    case HordeSatSolverConfig_Diversification_SPARSE_NATIVE:
      sparseDiversification(mpi_size, mpi_rank, _solver_ptrs);
      nativeDiversification(mpi_rank, mpi_size, _solver_ptrs);
      break;
    case HordeSatSolverConfig_Diversification_SPARSE_RANDOM:
      sparseRandomDiversification(_cfg.seed(), mpi_size, _solver_ptrs);
      break;
    case HordeSatSolverConfig_Diversification_SPARSE_RANDOM_NATIVE:
      sparseRandomDiversification(_cfg.seed(), mpi_size, _solver_ptrs);
      nativeDiversification(mpi_rank, mpi_size, _solver_ptrs);
      break;
    default:
      IPS_TERMINATE("HordeSat: invalid diversification mode.");
  }
}

void HordeSatSolver::parse_cnf(std::filesystem::path const& path) {
  _prop.parse_cnf(path);
  loadFormulaToSolvers(_solver_ptrs, path.c_str());
  _diversify();
}

State HordeSatSolver::solve(const Minisat::vec<Minisat::Lit>& assumptions) {
  // clear ending detection
  std::fill(_ending_buffer.get(), _ending_buffer.get() + mpi_size, 0);
  _result = UNKNOWN;
  _solvingDoneLocal = false;

#define INT_LIT(lit) Minisat::sign(lit) ? -(Minisat::var(lit) + 1) : (Minisat::var(lit) + 1)

  {  // start solvers
    std::lock_guard<std::mutex> lg(_slv_mutex);
    _assumptions.resize(assumptions.size());
    for (size_t i = 0; i < assumptions.size(); ++i) {
      _assumptions[i] = INT_LIT(assumptions[(int) i]);
    }
    ++_solve_idx;
  }
  _slv_cv.notify_all();

  uint32_t share_interval = _cfg.sharing_interval_us();
  uint32_t sleep_between = _cfg.wakeup_interval_us();
  uint32_t steps_share_interval = share_interval / sleep_between;
  uint32_t cur_step = 0;

  // wait for result
  while (!getGlobalEnding(
      mpi_size, mpi_rank, _solvingDoneLocal, _ending_buffer.get(), _interruptLock, _solver_ptrs)) {
    usleep(sleep_between);
    ++cur_step;
    if (cur_step == _cfg.max_rounds()) {
      _solvingDoneLocal = true;
    }
    if ((cur_step++ % steps_share_interval) == 0 && _sharing_manager) {
      _sharing_manager->doSharing();
    }
  }

  return _result;
}

unsigned HordeSatSolver::num_vars() const noexcept {
  return _solvers.front()->getVariablesCount();
}

bool HordeSatSolver::propagate_confl(Minisat::vec<Minisat::Lit> const& assumptions) {
  return _prop.propagate(assumptions);
}

REGISTER_PROTO(Solver, HordeSatSolver, hordesat_solver_config);

}  // namespace core::sat