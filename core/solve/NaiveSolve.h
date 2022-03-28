#ifndef ITMO_PARSAT_NAIVESOLVE_H
#define ITMO_PARSAT_NAIVESOLVE_H

#include "core/solve/Solve.h"
#include "util/SigHandler.h"
#include "util/TimeTracer.h"
#include "util/Logger.h"

namespace core::solve {

/**
 * @brief Uses the specified native solver to solve the problem.
 */
class NaiveSolve : public Solve {
 public:
  explicit NaiveSolve(NaiveSolveConfig config);

  [[nodiscard]] sat::State solve(sat::Problem const& problem) override;

 protected:
  void _interrupt_impl() override;

 private:
  sat::solver::RSolver _solver;
  NaiveSolveConfig _cfg;
};

}  // namespace core::solve

#endif  // ITMO_PARSAT_NAIVESOLVE_H
