#ifndef ITMO_PARSAT_FULLSEARCH_H
#define ITMO_PARSAT_FULLSEARCH_H

#include "core/search/Search.h"

namespace core::search {

/**
 * @brief The class used to perform full search.
 */
class FullSearch : public Search {
 public:
  FullSearch(domain::VarView const& var_view, bit_mask_t const& bit_mask);

  explicit FullSearch(std::vector<int> const& vars);

  [[nodiscard]] FullSearch* clone() const override;

 protected:
  void _advance() override;

  void _reset() override;
};

MAKE_REFS(FullSearch);

UFullSearch createFullSearch(domain::VarView const& var_view, bit_mask_t const& bit_mask);

UFullSearch createFullSearch(std::vector<int> const& vars);

}  // namespace core::search

#endif  // ITMO_PARSAT_FULLSEARCH_H
