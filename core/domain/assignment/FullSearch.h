#ifndef ITMO_PARSAT_FULLSEARCH_H
#define ITMO_PARSAT_FULLSEARCH_H

#include "core/domain/assignment/ModifyingSearch.h"

namespace core::domain {

/**
 * @brief The class used to perform full search.
 */
class FullSearch : public ModifyingSearch {
  friend USearch createFullSearch(VarView const&, std::vector<bool> const&);

 protected:
  FullSearch(VarView const& var_view, std::vector<bool> const& vars);

 protected:
  void _advance() override;

  void _reset() override;

  [[nodiscard]] FullSearch* clone() const override;
};

USearch createFullSearch(VarView const& var_view, std::vector<bool> const& vars);

}  // namespace core::domain

#endif  // ITMO_PARSAT_FULLSEARCH_H
