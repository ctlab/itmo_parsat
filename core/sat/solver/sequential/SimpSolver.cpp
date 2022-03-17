#include "SimpSolver.h"

namespace core::sat::solver {

void SimpSolver::load_problem(Problem const& problem) {
  static_cast<MinisatSimpBase*>(this)->load_problem(problem);
}

State SimpSolver::solve(vec_lit_t const& assumptions) {
  clearInterrupt();
  Minisat::lbool result = IPS_TRACE_N_V(
      "SimpSolver::solve", static_cast<Minisat::SimpSolver*>(this)->solveLimited(assumptions));
  if (result == Minisat::l_True) {
    return SAT;
  } else if (result == Minisat::l_False) {
    return UNSAT;
  } else {
    return UNKNOWN;
  }
}

void SimpSolver::interrupt() {
  static_cast<Minisat::SimpSolver*>(this)->interrupt();
}

void SimpSolver::clear_interrupt() {
  static_cast<Minisat::SimpSolver*>(this)->clearInterrupt();
}

unsigned SimpSolver::num_vars() const noexcept {
  return nVars();
}

REGISTER_SIMPLE(Solver, SimpSolver);

}  // namespace core::sat::solver
