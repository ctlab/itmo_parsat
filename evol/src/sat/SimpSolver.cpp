#include "evol/include/sat/SimpSolver.h"

#include <csignal>

#include "evol/include/util/GzFile.h"
#include "evol/include/util/Registry.h"
#include "evol/include/util/profile.h"
#include "minisat/core/Dimacs.h"
#include "minisat/utils/System.h"

namespace ea::sat {

SimpSolver::SimpSolver(SimpSolverConfig const& config)
    : impl_(
          config.grow(), config.clause_lim(), config.subsumption_lim(), config.simp_garbage_frac(),
          config.use_asymm(), config.use_rcheck(), config.use_elim(),
          config.cli_config().verbosity_level(), config.solver_config().var_decay(),
          config.solver_config().cla_decay(), config.solver_config().rnd_freq(),
          config.solver_config().rnd_seed(), config.solver_config().luby(),
          config.solver_config().ccmin_mode(), config.solver_config().phase_saving(),
          config.solver_config().rnd_init(), config.solver_config().gc_frac(),
          config.solver_config().min_learnts(), config.solver_config().rfirst(),
          config.solver_config().rinc()),
      preprocess_(config.cli_config().preprocessing()) {
  impl_.verbosity = config.cli_config().verbosity_level();

  int cpu_lim = config.cli_config().cpu_limit();
  if (cpu_lim != 0) {
    Minisat::limitTime(cpu_lim);
  }

  int mem_lim = config.cli_config().mem_limit();
  if (mem_lim != 0) {
    Minisat::limitMemory(mem_lim);
  }
}

void SimpSolver::parse_cnf(std::filesystem::path const& path) {
  {
    util::GzFile gz_file(path);
    impl_.parsing = true;
    LOG_TIME(Minisat::parse_DIMACS(gz_file.native_handle(), impl_, true));
    impl_.parsing = false;
  }

  if (preprocess_) {
    LOG_TIME(impl_.eliminate(true));
  }

  if (!impl_.okay()) {
    state_ = UNSAT;
  }
}

State SimpSolver::state() const noexcept {
  return state_;
}

void SimpSolver::solve_limited(Minisat::vec<Minisat::Lit> const& assumptions) {
  LOG_TIME(Minisat::lbool result = impl_.solveLimited(assumptions));

  if (result == Minisat::l_True) {
    state_ = SAT;
  } else if (result == Minisat::l_False) {
    state_ = UNSAT;
  }
}

void SimpSolver::interrupt() {
  impl_.interrupt();
}

unsigned SimpSolver::num_vars() const noexcept {
  return impl_.nVars();
}

bool SimpSolver::propagate(
    Minisat::vec<Minisat::Lit> const& assumptions, Minisat::vec<Minisat::Lit>& propagated) {
  bool conflict = false;
  impl_.prop_check(assumptions, propagated, 0, conflict);
  return conflict;
}

REGISTER_PROTO(Solver, SimpSolver, simp_solver_config);

}  // namespace ea::sat
