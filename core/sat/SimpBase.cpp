#include "core/sat/SimpBase.h"

namespace core::sat {

SimpBase::SimpBase(SimpSolverConfig const& config)
    : Minisat::SimpSolver(
    config.grow(), config.clause_lim(), config.subsumption_lim(), config.simp_garbage_frac(),
    config.use_asymm(), config.use_rcheck(), config.use_elim(),
    config.minisat_cli_config().verbosity_level(), config.base_solver_config().var_decay(),
    config.base_solver_config().cla_decay(), config.base_solver_config().rnd_freq(),
    core::random::sample<int>(0, INT_MAX), config.base_solver_config().luby(),
    config.base_solver_config().ccmin_mode(), config.base_solver_config().phase_saving(),
    config.base_solver_config().rnd_init(), config.base_solver_config().gc_frac(),
    config.base_solver_config().min_learnts(), config.base_solver_config().rfirst(),
    config.base_solver_config().rinc()) {
  verbosity = config.minisat_cli_config().verbosity_level();
}

void SimpBase::parse_cnf(std::filesystem::path const& path) {
  {
    util::GzFile gz_file(path);
    parsing = true;
    Minisat::parse_DIMACS(gz_file.native_handle(), *this, true);
    parsing = false;
  }
  IPS_INFO_T(
      SOLVER_STATS_PREPROCESS, "Stats before preprocess: " << nClauses() << ' ' << nVars());
  eliminate(true);
  IPS_INFO_T(SOLVER_STATS_PREPROCESS, "Stats after preprocess: " << nClauses() << ' ' << nVars());
}

uint32_t SimpBase::num_vars() const noexcept {
  return nVars();
}

bool SimpBase::_propagate_confl(const Minisat::vec<Minisat::Lit>& assumptions) {
  return prop_check(assumptions, 0);
}

}  // namespace core::sat