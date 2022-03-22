#include "RecurringRBSSolve.h"

namespace core::solve {

RecurringRBSSolve::RecurringRBSSolve(RecurringRBSSolveConfig config)
    : RBSSolveBase(
          config.preprocess_config(), config.prop_config(),
          config.solver_service_config())
    , _cfg(std::move(config)) {}

sat::State RecurringRBSSolve::_solve_impl(
    const sat::Problem& problem,
    const ea::preprocess::RPreprocess& preprocess) {
  std::stack<std::pair<Mini::vec<Mini::Lit>, uint32_t>> stack;
  stack.push({});

  while (!stack.empty() && !_interrupted) {
    _cur_base_assumption = stack.top().first;
    uint32_t depth = stack.top().second;
    stack.pop();

    IPS_INFO(
        "Starting iteration on depth "
        << depth << " with base assumption: " << _cur_base_assumption);

    using namespace std::chrono_literals;
    auto time_limit =
        depth < 2 ? std::chrono::milliseconds(_cfg.max_solve_time_ms()) : 1h;

    // TODO(dzhiblavi@): maybe create algorithm once.
    ea::algorithm::RAlgorithm algorithm(
        ea::algorithm::AlgorithmRegistry::resolve(
            _cfg.algorithm_config(), _prop));
    algorithm->prepare(preprocess);
    algorithm->set_base_assumption(_cur_base_assumption);
    _do_interrupt = [algorithm] { algorithm->interrupt(); };
    IPS_TRACE(algorithm->process());
    auto const& rho_backdoor = algorithm->get_best();

    IPS_INFO("Number of points visited: " << algorithm->inaccurate_points());
    IPS_INFO("The best backdoor is: " << rho_backdoor);
    if (rho_backdoor.is_sbs()) {
      // UNSAT for this branch
      continue;
    }
    if (_interrupted) {
      return sat::UNKNOWN;
    }

    auto slow = _filter_fast(
        _filter_conflict(core::domain::createFullSearch(
            preprocess->var_view(), rho_backdoor.get_vars().get_mask())),
        time_limit);

    if (std::holds_alternative<sat::State>(slow) &&
        std::get<sat::State>(slow) == sat::SAT) {
      // Branch returned SAT, thus the result is SAT.
      return sat::SAT;
    } else if (!std::holds_alternative<sat::State>(slow)) {
      // Otherwise, fill the stack and continue search.
      for (auto& v : std::get<std::vector<Mini::vec<Mini::Lit>>>(slow)) {
        stack.emplace(std::move(v), depth + 1);
      }
    }

    IPS_INFO("Iteration finished. Stack size: " << stack.size());
  }

  return IPS_UNLIKELY(_interrupted) ? sat::UNKNOWN : sat::UNSAT;
}

RecurringRBSSolve::filter_r RecurringRBSSolve::_filter_fast(
    std::vector<Mini::vec<Mini::Lit>> const& assumptions,
    clock_t::duration time_limit) {
  IPS_INFO("Filtering slow assignments (" << assumptions.size() << ").");
  std::vector<Mini::vec<Mini::Lit>> failed;
  bool unknown = false, satisfied = false;
  boost::timer::progress_display progress(assumptions.size(), std::cerr);

  _do_interrupt = [this] { _solver_service->interrupt(); };
  IPS_TRACE_N("RecurringRBSSolve::_filter_fast", {
    std::vector<std::future<sat::State>> results;
    std::vector<Mini::vec<Mini::Lit>> cur_assumptions;
    cur_assumptions.reserve(assumptions.size());
    for (auto const& assumption : assumptions) {
      cur_assumptions.push_back(util::concat(_cur_base_assumption, assumption));
    }
    for (auto const& assumption : cur_assumptions) {
      results.push_back(_solver_service->solve(assumption, time_limit));
    }
    for (size_t i = 0; i < results.size(); ++i) {
      if (_interrupted) {
        break;
      }
      auto& result = results[i];
      switch (result.get()) {
        case core::sat::UNSAT:
          break;
        case core::sat::UNKNOWN:
          unknown = true;
          failed.push_back(std::move(cur_assumptions[i]));
          break;
        case core::sat::SAT:
          return sat::SAT;
      }
      ++progress;
    }
    IPS_INFO("Total: " << assumptions.size() << ", slow: " << failed.size());
  });

  if (!unknown && !_interrupted)
    return core::sat::UNSAT;

  if (failed.empty()) {
    return sat::UNKNOWN;
  } else {
    return failed;
  }
}

REGISTER_PROTO(Solve, RecurringRBSSolve, recurring_rbs_solve_config);

}  // namespace core::solve
