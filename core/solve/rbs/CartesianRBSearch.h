#ifndef ITMO_PARSAT_CARTESIANRBSEARCH_H
#define ITMO_PARSAT_CARTESIANRBSEARCH_H

#include "core/solve/rbs/RBSearch.h"
#include "core/proto/solve_config.pb.h"

#include "core/domain/assignment/CartesianSearch.h"
#include "core/evol/algorithm/Algorithm.h"
#include "util/EventHandler.h"
#include "util/Logger.h"

namespace core::solve {

/**
 * @brief Searches for multiple rho-backdoors with the specified parameters
 * and then performs cartesian product of non-conflicting sets. Finally, only
 * non-conflicting assumptions from the cartesian product are included in
 * the resulting search.
 */
class CartesianRBSearch : public RBSearch {
 public:
  CartesianRBSearch(
      CartesianRBSearchConfig config, sat::prop::RProp prop,
      ea::preprocess::RPreprocess preprocess);

  rbs_result_t find_rb() override;

 protected:
  void _interrupt_impl() override;

 private:
  void _raise_for_sbs(int id);

  std::vector<std::vector<std::vector<Mini::Lit>>> _pre_solve();

  domain::USearch _prepare_cartesian(
      std::vector<std::vector<std::vector<Mini::Lit>>>&& cartesian_set);

 private:
  std::atomic_bool _sbs_found{false};
  std::vector<ea::algorithm::RAlgorithm> _algorithms;
  std::mutex _m;
  CartesianRBSearchConfig _cfg;
};

}  // namespace core::solve

#endif  // ITMO_PARSAT_CARTESIANRBSEARCH_H
