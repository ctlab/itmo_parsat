#include "core/solve/RBSSolve.h"

namespace core {

RBSSolve::RBSSolve(RBSSolveConfig const& config) : _cfg(config) {}

sat::State RBSSolve::solve(sat::Problem const& problem) {
  domain::USearch final_search;
  {
    ea::algorithm::RAlgorithm algorithm(
        ea::algorithm::AlgorithmRegistry::resolve(_cfg.algorithm_config()));
    _do_interrupt = [algorithm] { algorithm->interrupt(); };
    auto& algorithm_solver = algorithm->get_prop();
    IPS_TRACE(algorithm_solver.load_problem(problem));
    if (!algorithm->prepare()) {
      final_search = domain::createSingleSearch();
    } else {
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
      final_search = core::domain::createFullSearch(
          algorithm->get_shared_data().var_view, rho_backdoor.get_vars().get_mask());
    }
    // Release algorithm and free resources.
    _do_interrupt = {};
  }
  auto solver = _resolve_solver(_cfg.solver_config());
  _do_interrupt = [solver] { solver->interrupt(); };
  IPS_TRACE(solver->load_problem(problem));
  return _final_solve(*solver, std::move(final_search));
}

REGISTER_PROTO(Solve, RBSSolve, rbs_solve_config);

}  // namespace core