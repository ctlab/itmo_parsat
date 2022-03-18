#include "MapleCOMSPSSolver.h"

namespace core::sat::solver {

void MapleCOMSPSSolver::load_problem(Problem const& problem) {
  static_cast<MapleCOMSPSSimpBase*>(this)->load_problem(problem);
}

State MapleCOMSPSSolver::solve(vec_lit_t const& assumptions) {
  clearInterrupt();

  MapleCOMSPS::lbool result = IPS_TRACE_N_V(
      "MapleCOMSPS::solve",
      static_cast<MapleCOMSPS::SimpSolver*>(this)->solveLimited(assumptions));
  if (result == MapleCOMSPS::l_True) {
    return SAT;
  } else if (result == MapleCOMSPS::l_False) {
    return UNSAT;
  } else {
    return UNKNOWN;
  }
}

void MapleCOMSPSSolver::interrupt() {
  static_cast<MapleCOMSPS::SimpSolver*>(this)->interrupt();
}

void MapleCOMSPSSolver::clear_interrupt() {
  static_cast<MapleCOMSPS::SimpSolver*>(this)->clearInterrupt();
}

unsigned MapleCOMSPSSolver::num_vars() const noexcept {
  return nVars();
}

REGISTER_SIMPLE(Solver, MapleCOMSPSSolver);

}  // namespace core::sat::solver
