#include "PainlessSolver.h"

#include <utility>

#include "util/options.h"

namespace core::sat::solver {

PainlessSolver::PainlessSolver(PainlessSolverConfig config)
    : _sharing(
          config.sharing_config().interval_us(),
          config.sharing_config().shr_lit())
    , _cfg(std::move(config)) {
  int cpus = _cfg.max_threads();
  int lbd_limit = _cfg.lbd_limit();
  int block_size = _cfg.block_size();
  int block_left = cpus % block_size, solvers = 0;
  int n_blocks = cpus / block_size + (block_left > 0);
  SolverInterface* solver = nullptr;

  _solver_block_list.resize(n_blocks);
  for (int i = 0; i < n_blocks; ++i, ++solvers) {
    auto& block = _solver_block_list[i];
    int cur_block_size = std::min(cpus - solvers, block_size);
    if (cur_block_size > 1) {
      for (int j = 0; j < cur_block_size - 1; ++j, ++solvers) {
        solver = painless::SolverFactory::createMapleCOMSPSSolver(lbd_limit);
        _solvers.push_back(solver);
        std::get<0>(block).push_back(solver);
      }
      solver = painless::SolverFactory::createReducerSolver(
          painless::SolverFactory::createMapleCOMSPSSolver(lbd_limit));
      _solvers.push_back(solver);
      std::get<1>(block) = solver;
    } else {
      solver = painless::SolverFactory::createMapleCOMSPSSolver(lbd_limit);
      _solvers.push_back(solver);
      std::get<0>(block).push_back(solver);
    }
  }

  working = std::make_unique<painless::Portfolio>(&_result);
  for (auto& _solver : _solvers) {
    working->addSlave(new painless::SequentialWorker(&_result, _solver));
  }
  if (_cfg.sharing_config().enabled()) {
    _sharing.share(_solver_block_list);
  }
}

PainlessSolver::~PainlessSolver() noexcept {
  _sharing.stop();
  working->setInterrupt();
  working->awaitStop();
  working.reset();
  std::for_each(
      IPS_EXEC_POLICY, _solvers.begin(), _solvers.end(),
      [](auto& solver) { delete solver; });
}

void PainlessSolver::load_problem(Problem const& problem) {
  std::for_each(
      IPS_EXEC_POLICY, _solvers.begin(), _solvers.end(),
      [&problem](auto& solver) { solver->loadFormula(problem.clauses()); });
  painless::SolverFactory::nativeDiversification(_solvers);
  painless::SolverFactory::sparseRandomDiversification(_solvers);
}

State PainlessSolver::solve(lit_vec_t const& assumptions) {
  clear_interrupt();
  ++solve_index;
  _result.globalEnding = false;
  _result.finalResult = PUNKNOWN;
  _result.finalModel.clear();

  working->solve(solve_index, assumptions, {});
  int slv_sleep_us = 100;
  while (!_result.globalEnding.load(std::memory_order_acquire) &&
         !_interrupted) {
    usleep(slv_sleep_us);
    if (slv_sleep_us < _cfg.slv_sleep_us()) {
      slv_sleep_us *= 2;
    }
  }
  working->setInterrupt();
  working->waitInterrupt();

  switch (_result.finalResult) {
    case PSAT:
      return SAT;
    case PUNSAT:
      return UNSAT;
    case PUNKNOWN:
      break;
  }
  return UNKNOWN;
}

unsigned PainlessSolver::num_vars() const noexcept {
  return _solvers.front()->getVariablesCount();
}

void PainlessSolver::interrupt() {
  _interrupted = true;
  working->setInterrupt();
}

void PainlessSolver::clear_interrupt() {
  _interrupted = false;
  working->unsetInterrupt();
}

sharing::SharingUnit PainlessSolver::sharing_unit() noexcept {
  return _solver_block_list;
}

REGISTER_PROTO(Solver, PainlessSolver, painless_solver_config);

}  // namespace core::sat::solver
