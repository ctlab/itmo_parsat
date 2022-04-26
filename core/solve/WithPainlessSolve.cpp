#include "WithPainlessSolve.h"

#include <utility>

namespace core::solve {

WithPainlessSolve::WithPainlessSolve(WithPainlessSolveConfig config)
    : _cfg(std::move(config))
    , _painless_solver(std::make_shared<sat::solver::PainlessSolver>(_cfg.painless_solver_config()))
    , _primary_solve(SolveRegistry::resolve(_cfg.near_solve_config())) {
  auto const& sharing_config = _cfg.sharing_config();
  if (sharing_config.enabled()) {
    _sharing.emplace(sharing_config.interval_us(), sharing_config.shr_lit());
    _sharing->share(*_painless_solver, *_primary_solve, sat::sharing::BIDIR);
  }
}

sat::State WithPainlessSolve::solve(sat::Problem const& problem) {
  std::atomic<sat::State> result{sat::UNKNOWN};

  std::thread t_painless([&, this] {
    sat::State expected = sat::UNKNOWN;
    _painless_solver->load_problem(problem);
    if (result.compare_exchange_strong(expected, _painless_solver->solve({}))) {
      IPS_INFO("Painless solver won.");
      _model = _painless_solver->get_model();
      _primary_solve->interrupt();
    }
  });
  std::thread t_solve([&, this] {
    util::random::Generator gen(_cfg.near_solve_config().random_seed());
    sat::State expected = sat::UNKNOWN;
    if (result.compare_exchange_strong(expected, _primary_solve->solve(problem))) {
      IPS_INFO("Primary solve won.");
      _model = _primary_solve->get_model();
      _painless_solver->interrupt();
    }
  });

  t_painless.join();
  t_solve.join();
  return result.load(std::memory_order_relaxed);
}

void WithPainlessSolve::_interrupt_impl() {
  _interrupt(_painless_solver);
  _interrupt(_primary_solve);
}

sat::sharing::SharingUnit WithPainlessSolve::sharing_unit() noexcept {
  IPS_WARNING("Requested sharing_unit from WithPainlessSolve.");
  return {};
}

Mini::vec<Mini::lbool> WithPainlessSolve::get_model() const { return _model; }

REGISTER_PROTO(Solve, WithPainlessSolve, with_painless_solve_config);

}  // namespace core::solve
