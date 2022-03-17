#ifndef ITMO_PARSAT_SOLVE_H
#define ITMO_PARSAT_SOLVE_H

#include <iostream>
#include <memory>
#include <atomic>

#include "core/sat/solver/sequential/Solver.h"
#include "util/EventHandler.h"
#include "util/SigHandler.h"
#include "util/TimeTracer.h"
#include "core/proto/solve_config.pb.h"

namespace core::solve {

/**
 * @brief The core class that performs all actions related to solving SAT.
 */
class Solve {
 public:
  Solve() = default;

  /**
   * @brief This method solves SAT. For now, it is expected to be called only once
   * per object lifetime.
   * @param input path to gz archive with task description
   * @return result of the solution
   */
  [[nodiscard]] virtual sat::State solve(sat::Problem const& problem) = 0;

  virtual ~Solve() = default;

  void interrupt();

 protected:
  [[nodiscard]] static sat::solver::RSolver _resolve_solver(SolverConfig const& config);

 protected:
  [[nodiscard]] bool _is_interrupted() const noexcept;

 protected:
  std::function<void()> _do_interrupt;
  std::atomic_bool _interrupted{false};
};

using RSolve = std::shared_ptr<Solve>;

DEFINE_REGISTRY(Solve, SolveConfig, solve);

}  // namespace core::solve

#endif  // ITMO_PARSAT_SOLVE_H
