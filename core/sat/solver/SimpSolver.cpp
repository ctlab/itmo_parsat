#include "core/sat/solver/SimpSolver.h"

#include "minisat/core/Dimacs.h"

namespace core::sat::solver {

SimpSolver::SimpSolver(SimpSolverConfig const& config) : SimpBase(config) {}

void SimpSolver::load_problem(Problem const& problem) {
  static_cast<SimpBase*>(this)->load_problem(problem);
}

State SimpSolver::solve(Minisat::vec<Minisat::Lit> const& assumptions) {
  clearInterrupt();
  bool result = IPS_TRACE_N_V(
      "SimpSolver::solve",
      static_cast<Minisat::SimpSolver*>(this)->solve(assumptions, true, false));
  return result ? SAT : UNSAT;
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

REGISTER_PROTO(Solver, SimpSolver, simp_solver_config);

}  // namespace core::sat::solver
