#ifndef ITMO_PARSAT_RBSSOLVE_H
#define ITMO_PARSAT_RBSSOLVE_H

#include <cmath>
#include <boost/timer/progress_display.hpp>

#include "core/evol/algorithm/Algorithm.h"
#include "core/evol/method/preprocess/Preprocess.h"
#include "core/solve/Solve.h"
#include "util/SigHandler.h"
#include "util/TimeTracer.h"
#include "util/Logger.h"
#include "core/domain/assignment/FullSearch.h"
#include "core/domain/assignment/RandomSearch.h"
#include "core/solve/RBSSolveBase.h"

namespace core::solve {

/**
 * @brief This class solves SAT in the following way:
 * - Uses the specified (E|G)A to find rho-backdoor B;
 * - Solves SAT formula for each assignment of variables from B.
 */
class RBSSolve : public RBSSolveBase {
 public:
  explicit RBSSolve(RBSSolveConfig const& config);

 private:
  [[nodiscard]] sat::State _solve_impl(
      sat::Problem const& problem, ea::preprocess::RPreprocess const& preprocess) override;

 private:
  RBSSolveConfig _cfg;
};

}  // namespace core::solve

#endif  // ITMO_PARSAT_RBSSOLVE_H
