#include "core/solve/RBSSolve.h"

namespace core {

RBSSolve::RBSSolve(RBSSolveConfig const& config) : _cfg(config) {}

sat::State RBSSolve::solve(std::filesystem::path const& input) {
  ea::algorithm::RAlgorithm algorithm(
      ea::algorithm::AlgorithmRegistry::resolve(_cfg.algorithm_config()));
  _do_interrupt = [algorithm] { algorithm->interrupt(); };
  auto& algorithm_solver = algorithm->get_solver();
  IPS_TRACE(algorithm_solver.parse_cnf(input));
  algorithm->prepare();

  IPS_TRACE(algorithm->process());
  auto const& rho_backdoor = algorithm->get_best();
  IPS_INFO("Number of points visited: " << algorithm->inaccurate_points());
  IPS_INFO("The best backdoor is: " << rho_backdoor);

  if (rho_backdoor.is_sbs()) {
    IPS_INFO("SBS found, the result is UNSAT.");
    return sat::UNSAT;
  }
  if (_is_interrupted()) {
    return sat::UNKNOWN;
  }

  std::vector<bool> vars = rho_backdoor.get_vars().get_mask();
  core::domain::USearch assignment_p =
      core::domain::createFullSearch(algorithm->get_shared_data().var_view, vars);
  auto solver = _resolve_solver(_cfg.solver_config());
  _do_interrupt = [solver] { solver->interrupt(); };
  IPS_TRACE(solver->parse_cnf(input));
  return _final_solve(*solver, std::move(assignment_p));
}

REGISTER_PROTO(Solve, RBSSolve, rbs_solve_config);

}  // namespace core