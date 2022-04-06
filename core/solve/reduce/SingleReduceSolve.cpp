#include "SingleReduceSolve.h"

namespace core::solve {

SingleReduceSolve::SingleReduceSolve(SingleReduceSolveConfig config)
    : ReduceSolve(
          config.preprocess_config(), config.prop_config(),
          config.solver_service_config())
    , _cfg(std::move(config)) {}

void SingleReduceSolve::_interrupt_impl() {
  ReduceSolve::_interrupt_impl();
  _interrupt(_rb_search);
}

sat::State SingleReduceSolve::_solve_impl(
    ea::preprocess::RPreprocess const& preprocess) {
  _rb_search.reset(
      RBSearchRegistry::resolve(_cfg.rb_search_config(), _prop, _preprocess));
  return std::visit(
      overloaded{
          [](RBSReason stop_reason) -> sat::State {
            switch (stop_reason) {
              case RBS_SBS_FOUND:
                return sat::UNSAT;
              case RBS_INTERRUPTED:
                return sat::UNKNOWN;
            }
            return sat::UNKNOWN;
          },
          [this](search::USearch&& search) -> sat::State {
            return _solve_final(_filter_conflict(std::move(search)));
          }},
      _rb_search->find_rb());
}

REGISTER_PROTO(Solve, SingleReduceSolve, single_reduce_solve_config);

}  // namespace core::solve
