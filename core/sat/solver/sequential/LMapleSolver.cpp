#include "LMapleSolver.h"

namespace core::sat::solver {

LMapleSolver::LMapleSolver(LMapleSolverConfig const& config)
    : painless::LMapleSolver(config.lbd_limit(), config.sharing_enabled()) {}

State LMapleSolver::solve(lit_vec_t const& assumptions) {
  clear_interrupt();
  painless::PSatResult result = IPS_BLOCK_R(solver_solve, painless::LMapleSolver::solve(assumptions, {}));
  switch (result) {
    case painless::PSAT: return SAT;
    case painless::PUNSAT: return UNSAT;
    case painless::PUNKNOWN: return UNKNOWN;
  }
  return UNKNOWN;
}

void LMapleSolver::load_problem(Problem const& problem) { loadFormula(problem.clauses()); }

void LMapleSolver::interrupt() { painless::LMapleSolver::setSolverInterrupt(); }

void LMapleSolver::clear_interrupt() { painless::LMapleSolver::unsetSolverInterrupt(); }

unsigned LMapleSolver::num_vars() const noexcept { return nVars(); }

sharing::SharingUnit LMapleSolver::sharing_unit() noexcept { return sharing::SolverList({this}); }

Mini::vec<Mini::lbool> LMapleSolver::get_model() const noexcept { return painless::LMapleSolver::getModel(); }

REGISTER_PROTO(Solver, LMapleSolver, lmaple_solver_config);

}  // namespace core::sat::solver
