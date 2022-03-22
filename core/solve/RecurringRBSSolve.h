#ifndef ITMO_PARSAT_RECURRINGRBSSOLVE_H
#define ITMO_PARSAT_RECURRINGRBSSOLVE_H

#include <stack>
#include <variant>

#include "core/solve/RBSSolveBase.h"
#include "core/evol/algorithm/Algorithm.h"
#include "core/evol/method/preprocess/Preprocess.h"
#include "core/solve/Solve.h"
#include "util/SigHandler.h"
#include "util/TimeTracer.h"
#include "util/Logger.h"
#include "core/domain/assignment/FullSearch.h"
#include "core/domain/assignment/RandomSearch.h"
#include "core/solve/RBSSolveBase.h"
#include "util/mini.h"

namespace core::solve {

/**
 * @brief This class solves SAT in the following way:
 * - Uses the specified (E|G)A to find rho-backdoor B;
 * - Filters the assignments, forgetting ones that solve fast;
 * - For slow assignments, repeats the first step.
 */
class RecurringRBSSolve : public RBSSolveBase {
 private:
  using filter_r = std::variant<sat::State, std::vector<Mini::vec<Mini::Lit>>>;

 public:
  explicit RecurringRBSSolve(RecurringRBSSolveConfig config);

 protected:
  sat::State _solve_impl(
      sat::Problem const& problem,
      ea::preprocess::RPreprocess const& preprocess) override;

 private:
  filter_r _filter_fast(
      std::vector<Mini::vec<Mini::Lit>> const& assumptions,
      clock_t::duration time_limit);

 private:
  Mini::vec<Mini::Lit> _cur_base_assumption;
  RecurringRBSSolveConfig _cfg;
};

}  // namespace core::solve

#endif  // ITMO_PARSAT_RECURRINGRBSSOLVE_H
