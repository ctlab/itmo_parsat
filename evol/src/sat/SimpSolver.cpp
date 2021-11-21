#include "evol/include/sat/SimpSolver.h"

#include <csignal>

#include "evol/include/util/GzFile.h"
#include "evol/include/util/Registry.h"
#include "evol/include/util/profile.h"
#include "minisat/core/Dimacs.h"
#include "minisat/utils/System.h"

namespace {

void unset_sigs() {
  LOG(INFO) << "Setting signal handlers to default.";
  std::signal(SIGINT, SIG_DFL);
  std::signal(SIGTERM, SIG_DFL);
}

ea::sat::Solver* solver_ = nullptr;

void interrupt_solver(int) {
  if (solver_ != nullptr) {
    LOG(INFO) << "Caught interrupt on working solver.";
    solver_->interrupt();
    unset_sigs();
  }
}

void set_sigs(ea::sat::Solver* solver) {
  LOG(INFO) << "Setting signal handlers to solver interrupt.";
  solver_ = solver;
  std::signal(SIGINT, interrupt_solver);
  std::signal(SIGTERM, interrupt_solver);
}

}  // namespace

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
  if (state_ != UNKNOWN) {
    LOG(INFO) << "State is not UNKNOWN, aborting solve.";
    return;
  }

  set_sigs(this);
  LOG_TIME(Minisat::lbool result = impl_.solveLimited(assumptions));
  unset_sigs();

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
    Minisat::vec<Minisat::Lit> const& assumps, Minisat::vec<Minisat::Lit>& props) {
  return impl_.prop_check(assumps, props, 0);
}

REGISTER_PROTO(Solver, SimpSolver, simp_solver_config);

}  // namespace ea::sat
