#include "PainlessSolver.h"

namespace core::sat::solver {

PainlessSolver::PainlessSolver(PainlessSolverConfig const& config) : _cfg(config) {
  int cpus = _cfg.max_threads();
  int shr_strat = 1;
  int shr_sleep_us = _cfg.shr_sleep_us();
  int timeout_s = _cfg.time_limit_s();
  int lbd_limit = _cfg.lbd_limit();
  int shr_lit = _cfg.shr_lit();
  int max_memory = _cfg.max_memory();
  int slv_sleep_us = _cfg.slv_sleep_us();

  painless::SolverFactory::createMapleCOMSPSSolvers(lbd_limit, max_memory, cpus - 2, _solvers);
  _solvers.push_back(painless::SolverFactory::createReducerSolver(
      painless::SolverFactory::createMapleCOMSPSSolver(lbd_limit)));
  _solvers.push_back(painless::SolverFactory::createReducerSolver(
      painless::SolverFactory::createMapleCOMSPSSolver(lbd_limit)));

  int nSolvers = _solvers.size();

  for (int id = 0; id < nSolvers; id++) {
    if (id % 2) {
      _solvers_LRB.push_back(_solvers[id]);
    } else {
      _solvers_VSIDS.push_back(_solvers[id]);
    }
  }

  painless::SolverFactory::nativeDiversification(_solvers);
  painless::SolverFactory::sparseRandomDiversification(_solvers_LRB);
  painless::SolverFactory::sparseRandomDiversification(_solvers_VSIDS);

  working = std::make_unique<painless::Portfolio>(&_result);
  {
    prod1.insert(prod1.end(), _solvers.begin(), _solvers.begin() + (cpus / 2 - 1));
    prod1.push_back(_solvers[_solvers.size() - 2]);
    prod2.insert(prod2.end(), _solvers.begin() + (cpus / 2 - 1), _solvers.end() - 2);
    prod2.push_back(_solvers[_solvers.size() - 1]);
    cons1.insert(cons1.end(), _solvers.begin(), _solvers.end() - 1);
    cons2.insert(cons2.end(), _solvers.begin(), _solvers.end() - 2);
    cons2.push_back(_solvers[_solvers.size() - 1]);

    nSharers = 2;
    _sharers.resize(nSharers);
    _sharers[0] = std::make_unique<painless::Sharer>(
        shr_sleep_us, 1, new painless::HordeSatSharing(shr_lit, shr_sleep_us, &_result), prod1,
        cons1);
    _sharers[1] = std::make_unique<painless::Sharer>(
        shr_sleep_us, 2, new painless::HordeSatSharing(shr_lit, shr_sleep_us, &_result), prod2,
        cons2);
  }

  for (size_t i = 0; i < nSolvers; i++) {
    working->addSlave(new painless::SequentialWorker(&_result, _solvers[i]));
  }
}

PainlessSolver::~PainlessSolver() noexcept {
  // Stop sharing first
  _sharers.clear();

  // Stop working
  interrupt();
  working->awaitStop();
  working.reset();

  // cleanup
  for (auto solver : _solvers) {
    delete solver;
  }
}

void PainlessSolver::load_problem(Problem const& problem) {
  for (auto solver : _solvers) {
    solver->loadFormula(problem.path().c_str());
  }
}

State PainlessSolver::solve(vec_lit_t const& assumptions) {
  // reset the result
  _result.globalEnding = false;
  _result.finalResult = PUNKNOWN;

  vector<int> cube;
  working->solve(cube);

  uint32_t slv_sleep_us = 100;
  uint32_t timeout_s = _cfg.time_limit_s();

  while (!_result.globalEnding) {
    usleep(slv_sleep_us);
    if (timeout_s > 0 && getRelativeTime() >= timeout_s) {
      _result.globalEnding = true;
      working->setInterrupt();
    }
    if (slv_sleep_us < _cfg.slv_sleep_us()) {
      slv_sleep_us *= 2;
    }
  }

  switch (working->result->finalResult) {
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

bool PainlessSolver::propagate_confl(const vec_lit_t& assumptions) {
  return false;
}

void PainlessSolver::_do_interrupt() {
  working->setInterrupt();
}

void PainlessSolver::_do_clear_interrupt() {
  working->unsetInterrupt();
}

REGISTER_PROTO(Solver, PainlessSolver, painless_solver_config);

}  // namespace core::sat::solver
