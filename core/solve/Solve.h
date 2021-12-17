#ifndef ITMO_PARSAT_SOLVE_H
#define ITMO_PARSAT_SOLVE_H

#include <iostream>
#include <memory>

#include "core/sat/Solver.h"
#include "core/proto/solve_config.pb.h"

namespace core {

class Solve {
 public:
  [[nodiscard]] virtual sat::State solve(std::filesystem::path const& input) = 0;

 protected:
  [[nodiscard]] static sat::RSolver _resolve_solver(SolverConfig const& config);
};

using RSolve = std::shared_ptr<Solve>;

DEFINE_REGISTRY(Solve, SolveConfig, solve);

}  // namespace core

#endif  // ITMO_PARSAT_SOLVE_H
