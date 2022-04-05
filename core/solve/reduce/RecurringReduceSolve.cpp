#include "RecurringReduceSolve.h"

namespace core::solve {

RecurringReduceSolve::RecurringReduceSolve(RecurringReduceSolveConfig config)
    : ReduceSolve(
          config.preprocess_config(), config.prop_config(),
          config.solver_service_config())
    , _cfg(std::move(config)) {}

void RecurringReduceSolve::_interrupt_impl() {
  ReduceSolve::_interrupt_impl();
  _interrupt(_rb_search);
}

sat::State RecurringReduceSolve::_solve_impl(
    ea::preprocess::RPreprocess const& preprocess) {
  std::stack<std::pair<lit_vec_t, uint32_t>> stack;
  stack.push({});

  while (!stack.empty() && !_is_interrupted()) {
    _cur_base_assumption = stack.top().first;
    uint32_t depth = stack.top().second;
    stack.pop();

    IPS_INFO(
        "Starting iteration on depth "
        << depth << " with base assumption: " << _cur_base_assumption);

    using namespace std::chrono_literals;
    auto time_limit = depth < _cfg.max_recursion_depth()
                          ? std::chrono::milliseconds(_cfg.max_solve_time_ms())
                          : sat::solver::SolverService::DUR_INDEF;

    _rb_search.reset(
        RBSearchRegistry::resolve(_cfg.rb_search_config(), _prop, _preprocess));
    sat::State result = sat::UNKNOWN;
    filter_r slow;
    std::visit(
        overloaded{
            [&result](RBSReason stop_reason) {
              switch (stop_reason) {
                case RBS_SBS_FOUND:
                  result = sat::UNSAT;
                  break;
                case RBS_INTERRUPTED:
                  result = sat::UNKNOWN;
                  break;
              }
            },
            [this, &slow, &time_limit](domain::USearch&& search) {
              slow =
                  _filter_fast(_filter_conflict(std::move(search)), time_limit);
            }},
        _rb_search->find_rb());

    if (_is_interrupted() || result != sat::UNKNOWN) {
      return result;
    }

    if (std::holds_alternative<sat::State>(slow) &&
        std::get<sat::State>(slow) == sat::SAT) {
      // Branch returned SAT, thus the result is SAT.
      return sat::SAT;
    } else if (!std::holds_alternative<sat::State>(slow)) {
      // Otherwise, fill the stack and continue search.
      for (auto& v : std::get<std::vector<lit_vec_t>>(slow)) {
        stack.emplace(std::move(v), depth + 1);
      }
    }

    IPS_INFO("Iteration finished. Stack size: " << stack.size());
  }

  return IPS_UNLIKELY(_is_interrupted()) ? sat::UNKNOWN : sat::UNSAT;
}

RecurringReduceSolve::filter_r RecurringReduceSolve::_filter_fast(
    std::vector<lit_vec_t> const& assumptions, clock_t::duration time_limit) {
  IPS_INFO("Filtering slow assignments (" << assumptions.size() << ").");
  std::atomic_bool unknown{false}, satisfied{false};
  std::vector<lit_vec_t> failed;
  std::mutex progress_lock, failed_lock;
  boost::timer::progress_display progress(assumptions.size(), std::cerr);

  IPS_TRACE_N("RecurringRBSSolve::_filter_fast", {
    std::vector<std::future<sat::State>> futures;
    futures.reserve(assumptions.size());
    std::vector<lit_vec_t> cur_assumptions;
    cur_assumptions.reserve(assumptions.size());
    for (auto const& assumption : assumptions) {
      cur_assumptions.push_back(util::concat(_cur_base_assumption, assumption));
    }
    for (size_t i = 0; i < cur_assumptions.size(); ++i) {
      futures.push_back(_solver_service->solve(
          cur_assumptions[i], time_limit, [&, i](sat::State result) {
            {
              std::lock_guard<std::mutex> lg(progress_lock);
              ++progress;
            }
            switch (result) {
              case sat::UNSAT:
                break;
              case sat::UNKNOWN:
                unknown = true;
                {
                  std::lock_guard<std::mutex> lg(failed_lock);
                  failed.push_back(std::move(cur_assumptions[i]));
                }
                break;
              case sat::SAT:
                satisfied = true;
                _solver_service->interrupt();
                break;
            }
          }));
    }
    util::wait_for_futures(futures);
    IPS_INFO("Total: " << assumptions.size() << ", slow: " << failed.size());
  });

  if (satisfied) {
    return sat::SAT;
  } else if (!unknown && !_is_interrupted()) {
    return sat::UNSAT;
  } else if (failed.empty()) {
    return sat::UNKNOWN;
  } else {
    return failed;
  }
}

REGISTER_PROTO(Solve, RecurringReduceSolve, recurring_reduce_solve_config);

}  // namespace core::solve