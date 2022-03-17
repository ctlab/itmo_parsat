#ifndef ITMO_PARSAT_SOLVERSERVICE_H
#define ITMO_PARSAT_SOLVERSERVICE_H

#include <future>

#include "core/sat/solver/sequential/Solver.h"

namespace core::sat::solver {

class SolverService {
 public:
  static constexpr clock_t::duration DUR_INDEF = std::chrono::hours(24);

 public:
  virtual ~SolverService() = default;

  virtual std::future<core::sat::State> solve(
      Mini::vec<Mini::Lit> const& assumptions,
      clock_t::duration time_limit = DUR_INDEF) = 0;

  virtual void interrupt() = 0;

  virtual void clear_interrupt() = 0;

  virtual void load_problem(sat::Problem const& problem) = 0;
};

DEFINE_REGISTRY(SolverService, SolverServiceConfig, solver_service);

MAKE_REFS(SolverService);

}  // namespace core::sat::solver

#endif  // ITMO_PARSAT_SOLVERSERVICE_H
