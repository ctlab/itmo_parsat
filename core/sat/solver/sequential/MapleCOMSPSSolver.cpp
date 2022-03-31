#include "MapleCOMSPSSolver.h"

namespace core::sat::solver {

MapleCOMSPSSolver::MapleCOMSPSSolver() : painless::MapleCOMSPSSolver(2) {}

State MapleCOMSPSSolver::solve(lit_vec_t const& assumptions) {
  clear_interrupt();
  PSatResult result = IPS_TRACE_N_V(
      "MapleCOMSPS::solve",
      painless::MapleCOMSPSSolver::solve(assumptions, {}));
  switch (result) {
    case PSAT:
      return SAT;
    case PUNSAT:
      return UNSAT;
    case PUNKNOWN:
      return UNKNOWN;
  }
  return UNKNOWN;
}

void MapleCOMSPSSolver::load_problem(Problem const& problem) {
  loadFormula(problem.clauses());
}

void MapleCOMSPSSolver::interrupt() {
  MapleCOMSPS::SimpSolver::interrupt();
}

void MapleCOMSPSSolver::clear_interrupt() {
  MapleCOMSPS::SimpSolver::clearInterrupt();
}

unsigned MapleCOMSPSSolver::num_vars() const noexcept {
  return nVars();
}

sharing::SharingUnit MapleCOMSPSSolver::sharing_unit() noexcept {
  return sharing::SolverList({this});
}

REGISTER_SIMPLE(Solver, MapleCOMSPSSolver);

}  // namespace core::sat::solver
