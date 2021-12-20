#include "core/domain/assignment/ModifierAssignment.h"

#include <iostream>

namespace core::domain {

ModifierAssignment::ModifierAssignment(
    VarView const& var_view, std::vector<bool> const& vars, size_t total)
    : Assignment(total) {
  for (int i = 0; i < (int) vars.size(); ++i) {
    if (vars[i]) {
      _assignment.push(Minisat::mkLit(var_view[i], false));
    }
  }
}

ModifierAssignment::ModifierAssignment(VarView const& var_view, std::vector<bool> const& vars)
    : ModifierAssignment(var_view, vars, 1ULL << std::count(vars.begin(), vars.end(), true)) {}

Minisat::vec<Minisat::Lit> const& ModifierAssignment::operator()() const {
  return _assignment;
}

}  // namespace core::domain
