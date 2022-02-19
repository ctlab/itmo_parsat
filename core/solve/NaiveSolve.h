#ifndef ITMO_PARSAT_NAIVESOLVE_H
#define ITMO_PARSAT_NAIVESOLVE_H

#include "core/solve/Solve.h"
#include "util/SigHandler.h"
#include "util/TimeTracer.h"
#include "util/Logger.h"

namespace core {

/**
 * @brief Naively solves SAT using the specified solver.
 */
class NaiveSolve : public Solve {
 public:
  NaiveSolve(NaiveSolveConfig config);

  [[nodiscard]] sat::State solve(sat::Problem const& problem) override;

 private:
  NaiveSolveConfig _cfg;
};

}  // namespace core

#endif  // ITMO_PARSAT_NAIVESOLVE_H
