#include "MapleCOMSPSSolver.h"

namespace core::sat::solver {

State MapleCOMSPSSolver::solve(lit_vec_t const& assumptions) {
  clear_interrupt();
  MapleCOMSPS::lbool result = IPS_TRACE_N_V(
      "MapleCOMSPS::solve",
      static_cast<MapleCOMSPS::SimpSolver*>(this)->solveLimited(
          assumptions, false, true));
  if (result == MapleCOMSPS::l_True) {
    return SAT;
  } else if (result == MapleCOMSPS::l_False) {
    return UNSAT;
  } else {
    return UNKNOWN;
  }
}

void MapleCOMSPSSolver::load_problem(Problem const& problem) {
  MapleCOMSPSSimpBase::load_problem(problem);
}

void MapleCOMSPSSolver::interrupt() {
  MapleCOMSPS::SimpSolver::interrupt();
}

void MapleCOMSPSSolver::clear_interrupt() {
  MapleCOMSPS::SimpSolver::clearInterrupt();
}

unsigned MapleCOMSPSSolver::num_vars() const noexcept {
  return MapleCOMSPSSimpBase::num_vars();
}

REGISTER_SIMPLE(Solver, MapleCOMSPSSolver);

}  // namespace core::sat::solver
