#include "core/solve/NaiveSolve.h"

#include <utility>

namespace core::solve {

NaiveSolve::NaiveSolve(NaiveSolveConfig config) : _cfg(std::move(config)) {}

void NaiveSolve::_interrupt_impl() {
  _interrupt(_solver);
}

sat::State NaiveSolve::solve(sat::Problem const& problem) {
  _solver = _resolve_solver(_cfg.solver_config());
  IPS_BLOCK(solver_load_problem, _solver->load_problem(problem));
  return IPS_BLOCK_R(solver_solve, _solver->solve());
}

sat::sharing::SharingUnit NaiveSolve::sharing_unit() noexcept {
  return _solver->sharing_unit();
}

REGISTER_PROTO(Solve, NaiveSolve, naive_solve_config);

}  // namespace core::solve
