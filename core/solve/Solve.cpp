#include "core/solve/Solve.h"

namespace core::solve {

void Solve::interrupt() {
  _interrupted = true;
  _interrupt_impl();
}

void Solve::_interrupt_impl() {}

bool Solve::_is_interrupted() const noexcept {
  return _interrupted;
}

sat::solver::RSolver Solve::_resolve_solver(SolverConfig const& config) {
  return sat::solver::RSolver(sat::solver::SolverRegistry::resolve(config));
}

}  // namespace core::solve