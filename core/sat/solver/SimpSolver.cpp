#include "core/sat/solver/SimpSolver.h"

#include "core/util/GzFile.h"
#include "core/util/Logger.h"
#include "core/util/Generator.h"
#include "minisat/core/Dimacs.h"
#include "minisat/utils/System.h"

namespace core::sat {

SimpSolver::SimpSolver(SimpSolverConfig const& config)
    : Minisat::SimpSolver(
          config.grow(), config.clause_lim(), config.subsumption_lim(), config.simp_garbage_frac(),
          config.use_asymm(), config.use_rcheck(), config.use_elim(),
          config.minisat_cli_config().verbosity_level(), config.base_solver_config().var_decay(),
          config.base_solver_config().cla_decay(), config.base_solver_config().rnd_freq(),
          core::random::sample<int>(0, INT_MAX), config.base_solver_config().luby(),
          config.base_solver_config().ccmin_mode(), config.base_solver_config().phase_saving(),
          config.base_solver_config().rnd_init(), config.base_solver_config().gc_frac(),
          config.base_solver_config().min_learnts(), config.base_solver_config().rfirst(),
          config.base_solver_config().rinc())
    , preprocess_(config.minisat_cli_config().preprocessing()) {
  verbosity = config.minisat_cli_config().verbosity_level();
}

void SimpSolver::parse_cnf(std::filesystem::path const& path) {
  {
    util::GzFile gz_file(path);
    parsing = true;
    Minisat::parse_DIMACS(gz_file.native_handle(), *this, true);
    parsing = false;
  }

  if (preprocess_) {
    IPS_INFO_T(
        SOLVER_STATS_PREPROCESS, "Stats before preprocess: " << nClauses() << ' ' << nVars());
    eliminate(true);
    IPS_INFO_T(SOLVER_STATS_PREPROCESS, "Stats after preprocess: " << nClauses() << ' ' << nVars());
  }
}

State SimpSolver::solve(Minisat::vec<Minisat::Lit> const& assumptions) {
  clearInterrupt();
  bool result = IPS_TRACE_N_V(
      "SimpSolver::solve",
      static_cast<Minisat::SimpSolver*>(this)->solve(assumptions, true, false));
  return result ? SAT : UNSAT;
}

void SimpSolver::_do_interrupt() {
  static_cast<Minisat::SimpSolver*>(this)->interrupt();
}

void SimpSolver::_do_clear_interrupt() {
  static_cast<Minisat::SimpSolver*>(this)->clearInterrupt();
}

unsigned SimpSolver::num_vars() const noexcept {
  return nVars();
}

bool SimpSolver::propagate(
    Minisat::vec<Minisat::Lit> const& assumptions, Minisat::vec<Minisat::Lit>& propagated) {
  clearInterrupt();
  return IPS_TRACE_N_V("SimpSolver::propagate_prop", !prop_check(assumptions, propagated, 0));
}

bool SimpSolver::propagate(Minisat::vec<Minisat::Lit> const& assumptions) {
  clearInterrupt();
  return IPS_TRACE_N_V("SimpSolver::propagate_confl", !prop_check(assumptions, 0));
}

uint64_t SimpSolver::prop_tree(Minisat::vec<Minisat::Lit> const& vars, uint32_t head_size) {
  clearInterrupt();
  return IPS_TRACE_N_V("SimpSolver::prop_tree", prop_check_subtree(vars, head_size));
  //  return 0;
}

REGISTER_PROTO(Solver, SimpSolver, simp_solver_config);

}  // namespace core::sat
