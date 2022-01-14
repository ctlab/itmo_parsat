#ifndef ITMO_PARSAT_RANDOMSEARCH_H
#define ITMO_PARSAT_RANDOMSEARCH_H

#include "core/domain/assignment/ModifyingSearch.h"
#include "core/domain/assignment/UniqueSearch.h"
#include "core/util/Generator.h"

namespace core::domain {

/**
 * @brief The class used to perform random search.
 */
class RandomSearch : public ModifyingSearch {
  friend USearch createRandomSearch(VarView const& var_view, std::vector<bool> const&, uint64_t);

 protected:
  explicit RandomSearch(VarView const& var_view, std::vector<bool> const& vars, uint64_t total);

 protected:
  void _advance() override;

  void _reset() override;

  [[nodiscard]] RandomSearch* clone() const override;
};

USearch createRandomSearch(VarView const& var_view, std::vector<bool> const& vars, uint64_t total);

}  // namespace core::domain

#endif  // ITMO_PARSAT_RANDOMSEARCH_H
