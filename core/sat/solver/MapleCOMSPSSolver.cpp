#include "core/sat/solver/MapleCOMSPSSolver.h"

namespace core::sat::solver {

void MapleCOMSPSSolver::load_problem(Problem const& problem) {
  static_cast<MapleCOMSPSSimpBase*>(this)->load_problem(problem);
}

State MapleCOMSPSSolver::solve(vec_lit_t const& assumptions) {
  clearInterrupt();

#if 1
  MapleCOMSPS::lbool result = IPS_TRACE_N_V(
      "MapleCOMSPS::solve", static_cast<MapleCOMSPS::SimpSolver*>(this)->solveLimited(assumptions));
  if (result == MapleCOMSPS::l_True) {
    return SAT;
  } else if (result == MapleCOMSPS::l_False) {
    return UNSAT;
  } else {
    return UNKNOWN;
  }
#else
  //  verbosity = 2;
  bool result = IPS_TRACE_N_V(
      "SimpSolver::solve", static_cast<MS_NS::SimpSolver*>(this)->solve(assumptions, true, false));
  return result ? SAT : UNSAT;
#endif
}

void MapleCOMSPSSolver::_do_interrupt() {
  static_cast<MapleCOMSPS::SimpSolver*>(this)->interrupt();
}

void MapleCOMSPSSolver::_do_clear_interrupt() {
  static_cast<MapleCOMSPS::SimpSolver*>(this)->clearInterrupt();
}

unsigned MapleCOMSPSSolver::num_vars() const noexcept {
  return nVars();
}

bool MapleCOMSPSSolver::propagate_confl(vec_lit_t const& assumptions) {
  return IPS_TRACE_N_V("SimpSolver::propagate_confl", _propagate_confl(assumptions));
}

REGISTER_SIMPLE(Solver, MapleCOMSPSSolver);

}  // namespace core::sat::solver
