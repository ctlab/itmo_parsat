#ifndef ITMO_PARSAT_SOLVE_H
#define ITMO_PARSAT_SOLVE_H

#include <iostream>
#include <memory>
#include <atomic>

#include <boost/timer/progress_display.hpp>

#include "core/sat/solver/Solver.h"
#include "core/util/SigHandler.h"
#include "core/util/Tracer.h"
#include "core/proto/solve_config.pb.h"

namespace core {

/**
 * @brief The core class that performs all actions related to solving SAT.
 */
class Solve {
 public:
  /**
   * @brief This method solves SAT. For now, it is expected to be called only once
   * per object lifetime.
   * @param input path to gz archive with task description
   * @return result of the solution
   */
  [[nodiscard]] virtual sat::State solve(std::filesystem::path const& input) = 0;

  virtual ~Solve() = default;

 protected:
  [[nodiscard]] static sat::RSolver _resolve_solver(SolverConfig const& config);

  [[nodiscard]] static sat::State _final_solve(sat::Solver& solver, domain::USearch assignment);
};

using RSolve = std::shared_ptr<Solve>;

DEFINE_REGISTRY(Solve, SolveConfig, solve);

}  // namespace core

#endif  // ITMO_PARSAT_SOLVE_H
