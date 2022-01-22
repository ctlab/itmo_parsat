#include "core/solve/RBSSolve.h"

namespace core {

RBSSolve::RBSSolve(RBSSolveConfig const& config) : _cfg(config) {}

sat::State RBSSolve::solve(std::filesystem::path const& input) {
  ea::algorithm::RAlgorithm algorithm(
      ea::algorithm::AlgorithmRegistry::resolve(_cfg.algorithm_config()));
  auto& algorithm_solver = algorithm->get_solver();
  IPS_TRACE(algorithm_solver.parse_cnf(input));
  algorithm->prepare();

  SigHandler::handle_t alg_int_handle =
      core::sig::register_callback([&alg_int_handle, &algorithm](int) {
        algorithm->interrupt();
        IPS_INFO("Algorithm has been interrupted.");
        alg_int_handle->remove();
      });

  IPS_TRACE(algorithm->process());
  auto& rho_backdoor = algorithm->get_best();
  IPS_INFO("Number of points visited: " << algorithm->inaccurate_points());
  IPS_INFO("The best backdoor is: " << rho_backdoor);

  std::vector<bool> vars = rho_backdoor.get_vars().get_mask();
  core::domain::USearch assignment_p =
      core::domain::createFullSearch(algorithm->get_shared_data().var_view, vars);

  auto solver = _resolve_solver(_cfg.solver_config());
  IPS_TRACE(solver->parse_cnf(input));
  return _final_solve(*solver, std::move(assignment_p));
}

REGISTER_PROTO(Solve, RBSSolve, rbs_solve_config);

}  // namespace core