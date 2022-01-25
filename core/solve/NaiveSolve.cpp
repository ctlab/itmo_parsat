#include "core/solve/NaiveSolve.h"

#include <utility>

namespace core {

NaiveSolve::NaiveSolve(NaiveSolveConfig config) : _cfg(std::move(config)) {}

sat::State NaiveSolve::solve(std::filesystem::path const& input) {
  auto solver = _resolve_solver(_cfg.solver_config());
  _do_interrupt = [solver] { solver->interrupt(); };
  IPS_TRACE(solver->parse_cnf(input));
  return IPS_TRACE_V(solver->solve_limited());
}

REGISTER_PROTO(Solve, NaiveSolve, naive_solve_config);

}  // namespace core
