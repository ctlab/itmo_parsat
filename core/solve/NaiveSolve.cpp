#include "core/solve/NaiveSolve.h"

#include <utility>

namespace core {

NaiveSolve::NaiveSolve(NaiveSolveConfig config) : _cfg(std::move(config)) {}

sat::State NaiveSolve::solve(std::filesystem::path const& input) {
  auto solver = _resolve_solver(_cfg.solver_config());
  IPS_TRACE(solver->parse_cnf(input));

  SigHandler::handle_t slv_int_handle = core::sig::register_callback([&](int) {
    solver->interrupt();
    IPS_INFO("Solver has been interrupted.");
    slv_int_handle->remove();
  });
  core::sig::unset();
  return IPS_TRACE_V(solver->solve_limited());
}

REGISTER_PROTO(Solve, NaiveSolve, naive_solve_config);

}  // namespace core
