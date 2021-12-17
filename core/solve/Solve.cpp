#include "core/solve/Solve.h"

namespace core {

sat::RSolver Solve::_resolve_solver(SolverConfig const& config) {
  return sat::RSolver(sat::SolverRegistry::resolve(config));
}

}  // namespace core