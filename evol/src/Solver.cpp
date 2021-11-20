#include "evol/include/sat/Solver.h"

#include <csignal>

#include "evol/include/util/GzFile.h"
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

Solver::Solver(FullMinisatSolverConfig const& config)
  : impl_(
      config.simp_solver_config().grow(),
      config.simp_solver_config().clause_lim(),
      config.simp_solver_config().subsumption_lim(),
      config.simp_solver_config().simp_garbage_frac(),
      config.simp_solver_config().use_asymm(),
      config.simp_solver_config().use_rcheck(),
      config.simp_solver_config().use_elim(),
      config.minisat_cli_config().verbosity_level(),
      config.solver_config().var_decay(),
      config.solver_config().cla_decay(),
      config.solver_config().rnd_freq(),
      config.solver_config().rnd_seed(),
      config.solver_config().luby(),
      config.solver_config().ccmin_mode(),
      config.solver_config().phase_saving(),
      config.solver_config().rnd_init(),
      config.solver_config().gc_frac(),
      config.solver_config().min_learnts(),
      config.solver_config().rfirst(),
      config.solver_config().rinc()
    ),
    preprocess_(config.minisat_cli_config().preprocessing())
{
  impl_.verbosity = config.minisat_cli_config().verbosity_level();

  int cpu_lim = config.minisat_cli_config().cpu_limit();
  if (cpu_lim != 0) {
    Minisat::limitTime(cpu_lim);
  }

  int mem_lim = config.minisat_cli_config().mem_limit();
  if (mem_lim != 0) {
    Minisat::limitMemory(mem_lim);
  }
}

void Solver::parse_cnf(std::filesystem::path const& path) {
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

State Solver::state() const noexcept {
  return state_;
}

void Solver::solve_limited() {
  if (state_ != UNKNOWN) {
    LOG(INFO) << "State is not UNKNOWN, aborting solve.";
    return;
  }

  Minisat::vec<Minisat::Lit> dummy;
  set_sigs(this);
  LOG_TIME(Minisat::lbool result = impl_.solveLimited(dummy));
  unset_sigs();

  if (result == Minisat::l_True) {
    state_ = SAT;
  } else if (result == Minisat::l_False) {
    state_ = UNSAT;
  }
}

void Solver::interrupt() {
  impl_.interrupt();
}

}  // namespace ea::sat
