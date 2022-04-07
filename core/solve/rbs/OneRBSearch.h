#ifndef ITMO_PARSAT_ONERBSEARCH_H
#define ITMO_PARSAT_ONERBSEARCH_H

#include "core/solve/rbs/RBSearch.h"
#include "core/proto/solve_config.pb.h"

#include "core/evol/algorithm/Algorithm.h"
#include "util/Logger.h"

namespace core::solve {

/**
 * @brief Searches for rho-backdoor with the specified parameters.
 */
class OneRBSearch : public RBSearch {
 public:
  OneRBSearch(
      OneRBSearchConfig config, sat::prop::RProp prop,
      ea::preprocess::RPreprocess preprocess);

  rbs_result_t find_rb(lit_vec_t const& base_assumption) override;

 protected:
  void _interrupt_impl() override;

 private:
  ea::algorithm::RAlgorithm _algorithm;
  OneRBSearchConfig _cfg;
};

}  // namespace core::solve

#endif  // ITMO_PARSAT_ONERBSEARCH_H
