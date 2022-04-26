#ifndef ITMO_PARSAT_SOLVERSERVICE_H
#define ITMO_PARSAT_SOLVERSERVICE_H

#include <future>

#include "core/sat/solver/sequential/Solver.h"
#include "core/sat/sharing/Shareable.h"

namespace core::sat::solver {

/**
 * @brief SAT solver service used to asynchronously execute sequential solvers
 * with specified time limits.
 */
class SolverService : public sat::sharing::Shareable {
 public:
  static constexpr util::clock_t::duration DUR_INDEF = std::chrono::milliseconds(0);

 public:
  using slv_callback_t = std::function<void(sat::State, std::optional<Mini::vec<Mini::lbool>> model)>;

 public:
  virtual ~SolverService() = default;

  /**
   * @brief Solve with callback.
   * @param assumption the assumption passed to solver
   * @param time_limit after this period solver will be interrupted
   * @param callback the callback
   * @return the future of sat solver result
   */
  virtual std::future<core::sat::State> solve(
      lit_vec_t const& assumption, util::clock_t::duration time_limit, slv_callback_t const& callback = {}) = 0;

  /**
   * @brief Load problem to all solvers.
   * @param problem the problem to be loaded
   */
  virtual void load_problem(sat::Problem const& problem) = 0;

  /**
   * @brief Interrupt all solvers in the service.
   */
  virtual void interrupt() = 0;

  /**
   * @brief Clear interrupt for all solvers.
   */
  virtual void clear_interrupt() = 0;
};

DEFINE_REGISTRY(SolverService, SolverServiceConfig, solver_service);

MAKE_REFS(SolverService);

}  // namespace core::sat::solver

#endif  // ITMO_PARSAT_SOLVERSERVICE_H
