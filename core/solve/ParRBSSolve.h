#ifndef ITMO_PARSAT_PARRBSSOLVE_H
#define ITMO_PARSAT_PARRBSSOLVE_H

#include <thread>
#include <mutex>
#include <vector>

#include "core/solve/Solve.h"
#include "core/evol/algorithm/Algorithm.h"
#include "util/SigHandler.h"
#include "util/Logger.h"
#include "util/Assert.h"
#include "util/Random.h"
#include "core/domain/assignment/CartesianSearch.h"

namespace core {

/**
 * @brief This class solves SAT in the following way:
 * - LaunchesDao multiple RBSSolve tasks with the specified configurations, collecting rho-backdoors
 *   set { B_1, ..., B_s };
 * - For each backdoor set B_i it collects the set G_i of all assignments for which propagate finds
 *   no conflicts;
 * - Builds cartesian product G = x { G_i };
 * - Runs solver's solve for all assignments from G.
 */
class ParRBSSolve : public Solve {
 public:
  explicit ParRBSSolve(ParRBSSolveConfig config);

  [[nodiscard]] sat::State solve(sat::Problem const& problem) override;

 private:
  std::vector<std::vector<std::vector<Mini::Lit>>> _pre_solve(sat::Problem const& problem);

  domain::USearch _prepare_cartesian(
      std::vector<std::vector<std::vector<Mini::Lit>>>&& cartesian_set);

  void _raise_for_sbs(uint32_t algorithm_id) noexcept;

 private:
  ParRBSSolveConfig _cfg;
  std::atomic_bool _sbs_found{false};
};

}  // namespace core

#endif  // ITMO_PARSAT_PARRBSSOLVE_H
