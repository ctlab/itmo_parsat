#include "SimpSolver.h"

namespace core::sat::solver {

State SimpSolver::solve(lit_vec_t const& assumptions) {
  clear_interrupt();
  Minisat::lbool result = IPS_TRACE_N_V(
      "SimpSolver::solve",
      Minisat::SimpSolver::solveLimited(assumptions, false, true));
  if (result == Minisat::l_True) {
    return SAT;
  } else if (result == Minisat::l_False) {
    return UNSAT;
  } else {
    return UNKNOWN;
  }
}

void SimpSolver::load_problem(Problem const& problem) {
  MinisatSimpBase::load_problem(problem);
}

void SimpSolver::interrupt() {
  Minisat::SimpSolver::interrupt();
}

void SimpSolver::clear_interrupt() {
  Minisat::SimpSolver::clearInterrupt();
}

unsigned SimpSolver::num_vars() const noexcept {
  return MinisatSimpBase::num_vars();
}

sharing::SharingUnit SimpSolver::sharing_unit() noexcept {
  return {};
}

REGISTER_SIMPLE(Solver, SimpSolver);

}  // namespace core::sat::solver
