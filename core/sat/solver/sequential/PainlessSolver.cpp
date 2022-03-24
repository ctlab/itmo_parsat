#include "PainlessSolver.h"

#include <utility>

#include "util/options.h"

namespace core::sat::solver {

PainlessSolver::PainlessSolver(PainlessSolverConfig config)
    : _cfg(std::move(config)) {
  int cpus = (int) _cfg.max_threads();
  int shr_sleep_us = (int) _cfg.shr_sleep_us();
  int lbd_limit = (int) _cfg.lbd_limit();
  int shr_lit = (int) _cfg.shr_lit();
  int max_memory = (int) _cfg.max_memory();

  painless::SolverFactory::createMapleCOMSPSSolvers(
      lbd_limit, max_memory, cpus - 2, _solvers);
  _solvers.push_back(painless::SolverFactory::createReducerSolver(
      painless::SolverFactory::createMapleCOMSPSSolver(lbd_limit)));
  _solvers.push_back(painless::SolverFactory::createReducerSolver(
      painless::SolverFactory::createMapleCOMSPSSolver(lbd_limit)));

  int nSolvers = (int) _solvers.size();

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
    prod1.insert(
        prod1.end(), _solvers.begin(), _solvers.begin() + (cpus / 2 - 1));
    prod1.push_back(_solvers[_solvers.size() - 2]);
    prod2.insert(
        prod2.end(), _solvers.begin() + (cpus / 2 - 1), _solvers.end() - 2);
    prod2.push_back(_solvers[_solvers.size() - 1]);
    cons1.insert(cons1.end(), _solvers.begin(), _solvers.end() - 1);
    cons2.insert(cons2.end(), _solvers.begin(), _solvers.end() - 2);
    cons2.push_back(_solvers[_solvers.size() - 1]);

    nSharers = 2;
    _sharers.resize(nSharers);
    _sharers[0] = std::make_unique<painless::Sharer>(
        shr_sleep_us, 1,
        new painless::HordeSatSharing(shr_lit, shr_sleep_us, &_result), prod1,
        cons1);
    _sharers[1] = std::make_unique<painless::Sharer>(
        shr_sleep_us, 2,
        new painless::HordeSatSharing(shr_lit, shr_sleep_us, &_result), prod2,
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
  working->setInterrupt();
  working->awaitStop();
  working.reset();

  // cleanup
  std::for_each(
      IPS_EXEC_POLICY, _solvers.begin(), _solvers.end(),
      [](auto& solver) { delete solver; });
}

void PainlessSolver::load_problem(Problem const& problem) {
  std::for_each(
      IPS_EXEC_POLICY, _solvers.begin(), _solvers.end(),
      [&problem](auto& solver) { solver->loadFormula(problem.clauses()); });
}

State PainlessSolver::solve(lit_vec_t const& assumptions) {
  clear_interrupt();
  ++solve_index;

  // reset the result
  _result.globalEnding = false;
  _result.finalResult = PUNKNOWN;
  working->solve(solve_index, assumptions, {});

  uint32_t slv_sleep_us = 100;
  uint32_t timeout_s = _cfg.time_limit_s();

  while (!_result.globalEnding && !_interrupted) {
    usleep(slv_sleep_us);
    if (timeout_s > 0 && getRelativeTime() >= timeout_s) {
      _result.globalEnding = true;
      interrupt();
      break;
    }
    if (slv_sleep_us < _cfg.slv_sleep_us()) {
      slv_sleep_us *= 2;
    }
  }

  working->setInterrupt();
  IPS_TRACE_N("WaitInterrupt", working->waitInterrupt());

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

REGISTER_PROTO(Solver, PainlessSolver, painless_solver_config);

}  // namespace core::sat::solver
