#include "MapleCOMSPSSolver.h"

namespace core::sat::solver {

MapleCOMSPSSolver::MapleCOMSPSSolver(MapleCOMSPSSolverConfig const& config)
    : painless::MapleCOMSPSSolver(
          config.lbd_limit(), config.sharing_enabled()) {}

State MapleCOMSPSSolver::solve(lit_vec_t const& assumptions) {
  clear_interrupt();
  painless::PSatResult result = IPS_TRACE_N_V(
      "MapleCOMSPS::solve",
      painless::MapleCOMSPSSolver::solve(assumptions, {}));
  switch (result) {
    case painless::PSAT:
      return SAT;
    case painless::PUNSAT:
      return UNSAT;
    case painless::PUNKNOWN:
      return UNKNOWN;
  }
  return UNKNOWN;
}

void MapleCOMSPSSolver::load_problem(Problem const& problem) {
  loadFormula(problem.clauses());
}

void MapleCOMSPSSolver::interrupt() {
  painless::MapleCOMSPSSolver::setSolverInterrupt();
}

void MapleCOMSPSSolver::clear_interrupt() {
  painless::MapleCOMSPSSolver::unsetSolverInterrupt();
}

unsigned MapleCOMSPSSolver::num_vars() const noexcept {
  return nVars();
}

sharing::SharingUnit MapleCOMSPSSolver::sharing_unit() noexcept {
  return sharing::SolverList({this});
}

REGISTER_PROTO(Solver, MapleCOMSPSSolver, maplecomsps_solver_config);

}  // namespace core::sat::solver
