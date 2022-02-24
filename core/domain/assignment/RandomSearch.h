#ifndef ITMO_PARSAT_RANDOMSEARCH_H
#define ITMO_PARSAT_RANDOMSEARCH_H

#include "core/domain/assignment/AssignmentModifier.h"
#include "core/domain/assignment/UniqueSearch.h"
#include "core/domain/assignment/Search.h"
#include "util/Random.h"

namespace core::domain {

class RandomSearch;
MAKE_REFS(RandomSearch);

/**
 * @brief The class used to perform random search.
 */
class RandomSearch : AssignmentModifier, public Search {
  friend USearch createRandomSearch(VarView const& var_view, std::vector<bool> const&, uint64_t);

 public:
  [[nodiscard]] RandomSearch* clone() const override;

  [[nodiscard]] lit_vec_t const& operator()() const override;

 protected:
  explicit RandomSearch(VarView const& var_view, std::vector<bool> const& vars, uint64_t total);

 protected:
  void _advance() override;

  void _reset() override;
};

USearch createRandomSearch(VarView const& var_view, std::vector<bool> const& vars, uint64_t total);

}  // namespace core::domain

#endif  // ITMO_PARSAT_RANDOMSEARCH_H
