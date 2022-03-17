#ifndef ITMO_PARSAT_PARRBSSOLVE_H
#define ITMO_PARSAT_PARRBSSOLVE_H

#include <thread>
#include <mutex>
#include <vector>

#include "core/solve/RBSSolveBase.h"
#include "core/evol/algorithm/Algorithm.h"
#include "core/evol/method/preprocess/Preprocess.h"
#include "util/SigHandler.h"
#include "util/Logger.h"
#include "util/Assert.h"
#include "util/Random.h"
#include "core/domain/assignment/CartesianSearch.h"

namespace core::solve {

/**
 * @brief This class solves SAT in the following way:
 * - LaunchesDao multiple RBSSolve tasks with the specified configurations, collecting rho-backdoors
 *   set { B_1, ..., B_s };
 * - For each backdoor set B_i it collects the set G_i of all assignments for which propagate finds
 *   no conflicts;
 * - Builds cartesian product G = x { G_i };
 * - Runs solver's solve for all assignments from G.
 */
class ParRBSSolve : public RBSSolveBase {
 public:
  explicit ParRBSSolve(ParRBSSolveConfig config);

 private:
  [[nodiscard]] sat::State _solve_impl(
      sat::Problem const& problem, ea::preprocess::RPreprocess const& preprocess) override;

  std::vector<std::vector<std::vector<Mini::Lit>>> _pre_solve(
      sat::Problem const& problem, ea::preprocess::RPreprocess const& preprocess);

  domain::USearch _prepare_cartesian(
      std::vector<std::vector<std::vector<Mini::Lit>>>&& cartesian_set);

  void _raise_for_sbs(uint32_t algorithm_id) noexcept;

 private:
  ParRBSSolveConfig _cfg;
  std::atomic_bool _sbs_found{false};
};

}  // namespace core::solve

#endif  // ITMO_PARSAT_PARRBSSOLVE_H
