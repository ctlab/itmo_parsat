#ifndef ITMO_PARSAT_RANDOMSEARCH_H
#define ITMO_PARSAT_RANDOMSEARCH_H

#include "core/search/AssignmentModifier.h"
#include "core/search/UniqueSearch.h"
#include "core/domain/SearchSpace.h"
#include "core/search/Search.h"
#include "util/Random.h"

namespace core::search {

/**
 * @brief The class used to perform random search.
 */
class RandomSearch : AssignmentModifier, public Search {
 public:
  explicit RandomSearch(
      domain::VarView const& var_view, bit_mask_t const& bit_mask, uint64_t total);

  explicit RandomSearch(std::vector<int> const& vars, uint64_t total);

  [[nodiscard]] RandomSearch* clone() const override;

  [[nodiscard]] lit_vec_t const& operator()() const override;

 protected:
  void _advance() override;

  void _reset() override;
};

MAKE_REFS(RandomSearch);

USearch createRandomSearch(
    domain::VarView const& var_view, bit_mask_t const& bit_mask, uint64_t total);

USearch createRandomSearch(std::vector<int> const& vars, uint64_t total);

USearch createAutoRandomSearch(
    domain::VarView const& var_view, bit_mask_t const& bit_mask, uint64_t total);

USearch createAutoRandomSearch(std::vector<int> const& vars, uint64_t total);

}  // namespace core::search

#endif  // ITMO_PARSAT_RANDOMSEARCH_H
