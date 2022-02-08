#ifndef ITMO_PARSAT_PARRBSSOLVE_H
#define ITMO_PARSAT_PARRBSSOLVE_H

#include <thread>
#include <mutex>
#include <vector>

#include "core/solve/Solve.h"
#include "core/evol/algorithm/Algorithm.h"
#include "core/util/SigHandler.h"
#include "core/util/Logger.h"
#include "core/util/assert.h"
#include "core/util/Generator.h"
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

  [[nodiscard]] sat::State solve(std::filesystem::path const& input) override;

 private:
  std::vector<std::vector<std::vector<Minisat::Lit>>> _pre_solve(
      std::filesystem::path const& input);

  std::vector<Minisat::vec<Minisat::Lit>> _build_cartesian_product(
      std::vector<std::vector<std::vector<Minisat::Lit>>>&& non_conflict_assignments);

  void _raise_for_sbs(int algorithm_id) noexcept;

 private:
  ParRBSSolveConfig _cfg;
  std::atomic_bool _sbs_found{false};
  std::mutex kek;
};

}  // namespace core

#endif  // ITMO_PARSAT_PARRBSSOLVE_H
