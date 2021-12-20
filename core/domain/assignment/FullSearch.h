#ifndef ITMO_PARSAT_FULLSEARCH_H
#define ITMO_PARSAT_FULLSEARCH_H

#include "core/domain/assignment/ModifierAssignment.h"

namespace core::domain {

class FullSearch : public ModifierAssignment {
  friend UAssignment createFullSearch(VarView const&, std::vector<bool> const&);

 protected:
  FullSearch(VarView const& var_view, std::vector<bool> const& vars);

 protected:
  void _advance() override;

  void _reset() override;

  [[nodiscard]] FullSearch* clone() const override;
};

UAssignment createFullSearch(VarView const& var_view, std::vector<bool> const& vars);

}  // namespace core::domain

#endif  // ITMO_PARSAT_FULLSEARCH_H
