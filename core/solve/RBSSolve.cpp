#include "core/solve/RBSSolve.h"

namespace core::solve {

RBSSolve::RBSSolve(RBSSolveConfig const& config)
    : RBSSolveBase(config.preprocess_config(), config.prop_config(), config.solver_service_config())
    , _cfg(config) {}

sat::State RBSSolve::_solve_impl(
    sat::Problem const& problem, ea::preprocess::RPreprocess const& preprocess) {
  domain::USearch final_search;
  {
    ea::algorithm::RAlgorithm algorithm(
        ea::algorithm::AlgorithmRegistry::resolve(_cfg.algorithm_config(), _prop));
    algorithm->prepare(preprocess);
    _do_interrupt = [algorithm] { algorithm->interrupt(); };
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
    final_search =
        core::domain::createFullSearch(preprocess->var_view(), rho_backdoor.get_vars().get_mask());
    _do_interrupt = {};
  }
  return _solve_final(_filter_conflict(std::move(final_search)));
}

REGISTER_PROTO(Solve, RBSSolve, rbs_solve_config);

}  // namespace core::solve