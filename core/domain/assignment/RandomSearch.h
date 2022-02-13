#ifndef ITMO_PARSAT_RANDOMSEARCH_H
#define ITMO_PARSAT_RANDOMSEARCH_H

#include "core/domain/assignment/AssignmentModifier.h"
#include "core/domain/assignment/UniqueSearch.h"
#include "core/domain/assignment/SplittableSearch.h"
#include "core/util/Generator.h"

namespace core::domain {

class RandomSearch;
MAKE_REFS(RandomSearch);

/**
 * @brief The class used to perform random search.
 */
class RandomSearch : AssignmentModifier, public SplittableSearch {
  friend USplittableSearch createRandomSearch(
      VarView const& var_view, std::vector<bool> const&, uint64_t);

 public:
  [[nodiscard]] RandomSearch* clone() const override;

  [[nodiscard]] Minisat::vec<Minisat::Lit> const& operator()() const override;

 protected:
  explicit RandomSearch(VarView const& var_view, std::vector<bool> const& vars, uint64_t total);

 protected:
  void _advance() override;

  void _reset() override;
};

USplittableSearch createRandomSearch(
    VarView const& var_view, std::vector<bool> const& vars, uint64_t total);

}  // namespace core::domain

#endif  // ITMO_PARSAT_RANDOMSEARCH_H
