#ifndef ITMO_PARSAT_MODIFIERASSIGNMENT_H
#define ITMO_PARSAT_MODIFIERASSIGNMENT_H

#include <cmath>
#include <algorithm>

#include "core/domain/assignment/Assignment.h"

namespace core::domain {

class ModifierAssignment : public Assignment {
 public:
  ModifierAssignment(VarView const& var_view, std::vector<bool> const& vars, size_t total);

  ModifierAssignment(VarView const& var_view, std::vector<bool> const& vars);

  Minisat::vec<Minisat::Lit> const& operator()() const override;

 protected:
  Minisat::vec<Minisat::Lit> _assignment;
};

}  // namespace core::domain

#endif  // ITMO_PARSAT_MODIFIERASSIGNMENT_H
