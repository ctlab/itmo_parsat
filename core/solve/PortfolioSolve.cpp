#include "PortfolioSolve.h"

#include "util/options.h"

#include <utility>

namespace core::solve {

PortfolioSolve::PortfolioSolve(PortfolioSolveConfig config) : _cfg(std::move(config)) {
  uint32_t solve_count = _cfg.solve_configs_size();
  for (uint32_t i = 0; i < solve_count; ++i) {
    _solves.emplace_back(SolveRegistry::resolve(_cfg.solve_configs(i)));
  }

  auto const& sharing_config = _cfg.sharing_config();
  if (sharing_config.enabled()) {
    _sharing.emplace(sharing_config.interval_us(), sharing_config.shr_lit());
    for (uint32_t i = 0; i < solve_count; ++i) {
      _sharing->share(*_solves[i], *_solves[(i + 1) % solve_count], sat::sharing::RIGHT);
    }
  }
}

sat::State PortfolioSolve::solve(sat::Problem const& problem) {
  std::atomic<sat::State> result{sat::UNKNOWN};
  std::vector<std::thread> threads;
  for (uint32_t i = 0; i < _cfg.solve_configs_size(); ++i) {
    threads.emplace_back([this, &result, &problem, i] {
      util::random::Generator g(_cfg.solve_configs(i).random_seed());
      sat::State expected = sat::UNKNOWN;
      auto& solve = _solves[i];
      if (result.compare_exchange_strong(expected, solve->solve(problem))) {
        IPS_INFO("Solve #" << i << " won");
        _model = solve->get_model();
        interrupt();
      }
    });
  }
  for (auto& thread : threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
  return result.load(std::memory_order_relaxed);
}

void PortfolioSolve::_interrupt_impl() {
  std::for_each(IPS_EXEC_POLICY, _solves.begin(), _solves.end(), [](auto& solve) { solve->interrupt(); });
}

sat::sharing::SharingUnit PortfolioSolve::sharing_unit() noexcept {
  IPS_WARNING("Requested sharing_unit from PortfolioSolve.");
  return {};
}

Mini::vec<Mini::lbool> PortfolioSolve::get_model() const { return _model; }

REGISTER_PROTO(Solve, PortfolioSolve, portfolio_solve_config);

}  // namespace core::solve
