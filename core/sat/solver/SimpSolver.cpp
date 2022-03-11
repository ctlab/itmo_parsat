#include "core/sat/solver/SimpSolver.h"

namespace core::sat::solver {

void SimpSolver::load_problem(Problem const& problem) {
  static_cast<MinisatSimpBase*>(this)->load_problem(problem);
}

State SimpSolver::solve(vec_lit_t const& assumptions) {
  clearInterrupt();

#if 1
  Minisat::lbool result = IPS_TRACE_N_V(
      "SimpSolver::solve", static_cast<Minisat::SimpSolver*>(this)->solveLimited(assumptions));
  if (result == Minisat::l_True) {
    return SAT;
  } else if (result == Minisat::l_False) {
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

void SimpSolver::_do_interrupt() {
  static_cast<Minisat::SimpSolver*>(this)->interrupt();
}

void SimpSolver::_do_clear_interrupt() {
  static_cast<Minisat::SimpSolver*>(this)->clearInterrupt();
}

unsigned SimpSolver::num_vars() const noexcept {
  return nVars();
}

bool SimpSolver::propagate_confl(Minisat::vec<Minisat::Lit> const& assumptions) {
  return IPS_TRACE_N_V("SimpSolver::propagate_confl", _propagate_confl(assumptions));
}

REGISTER_SIMPLE(Solver, SimpSolver);

}  // namespace core::sat::solver
