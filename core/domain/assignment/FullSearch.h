#ifndef ITMO_PARSAT_FULLSEARCH_H
#define ITMO_PARSAT_FULLSEARCH_H

#include "core/domain/assignment/AssignmentModifier.h"
#include "core/domain/assignment/Search.h"

namespace core::domain {

class FullSearch;
MAKE_REFS(FullSearch);

/**
 * @brief The class used to perform full search.
 */
class FullSearch : AssignmentModifier, public Search {
  friend UFullSearch createFullSearch(VarView const&, std::vector<bool> const&);

 public:
  FullSearch(VarView const& var_view, std::vector<bool> const& vars);

  FullSearch(std::vector<int> const& vars);

  [[nodiscard]] FullSearch* clone() const override;

  [[nodiscard]] lit_vec_t const& operator()() const override;

 protected:
  void _advance() override;

  void _reset() override;
};

UFullSearch createFullSearch(VarView const& var_view, std::vector<bool> const& vars);

UFullSearch createFullSearch(std::vector<int> const& vars);

}  // namespace core::domain

#endif  // ITMO_PARSAT_FULLSEARCH_H
