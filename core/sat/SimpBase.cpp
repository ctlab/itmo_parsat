#include "core/sat/SimpBase.h"

namespace core::sat {

SimpBase::SimpBase(SimpSolverConfig const& config)
    : Minisat::SimpSolver(
          config.grow(), config.clause_lim(), config.subsumption_lim(), config.simp_garbage_frac(),
          config.use_asymm(), config.use_rcheck(), config.use_elim(),
          config.minisat_cli_config().verbosity_level(), config.base_solver_config().var_decay(),
          config.base_solver_config().cla_decay(), config.base_solver_config().rnd_freq(),
          util::random::sample<int>(0, INT_MAX), config.base_solver_config().luby(),
          config.base_solver_config().ccmin_mode(), config.base_solver_config().phase_saving(),
          config.base_solver_config().rnd_init(), config.base_solver_config().gc_frac(),
          config.base_solver_config().min_learnts(), config.base_solver_config().rfirst(),
          config.base_solver_config().rinc()) {
  verbosity = config.minisat_cli_config().verbosity_level();
}

uint32_t SimpBase::num_vars() const noexcept {
  return nVars();
}

bool SimpBase::_propagate_confl(const Minisat::vec<Minisat::Lit>& assumptions) {
  return !prop_check(assumptions, 0);
}

bool SimpBase::load_problem(Problem const& problem) {
  bool result = true;
  IPS_TRACE_N("load_problem", {
    auto& clauses = problem.clauses();
    for (const auto& clause : clauses) {
      Minisat::vec<Minisat::Lit> mcls;
      for (int lit : clause) {
        int var = abs(lit);
        while (nVars() < var) {
          newVar();
        }
        mcls.push(MINI_LIT(lit));
      }
      if (!addClause(mcls)) {
        IPS_INFO("The formula has been solved during initialization (UNSAT).");
        result = false;
      }
    }
  });
  IPS_TRACE(eliminate(true));
  return result;
}

}  // namespace core::sat