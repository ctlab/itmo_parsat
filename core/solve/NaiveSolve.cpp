#include "core/solve/NaiveSolve.h"

#include <utility>

namespace core {

NaiveSolve::NaiveSolve(NaiveSolveConfig config) : _cfg(std::move(config)) {}

sat::State NaiveSolve::solve(sat::Problem const& problem) {
  auto solver = _resolve_solver(_cfg.solver_config());
  _do_interrupt = [solver] { solver->interrupt(); };
  IPS_TRACE(solver->load_problem(problem));
  return IPS_TRACE_V(solver->solve_limited());
}

REGISTER_PROTO(Solve, NaiveSolve, naive_solve_config);

}  // namespace core
