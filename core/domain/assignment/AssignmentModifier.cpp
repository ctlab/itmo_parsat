#include "core/domain/assignment/AssignmentModifier.h"

namespace core::domain {

AssignmentModifier::AssignmentModifier(VarView const& var_view, std::vector<bool> const& vars) {
  for (int i = 0; i < (int) vars.size(); ++i) {
    if (vars[i]) {
      _assignment.push(Minisat::mkLit(var_view[i], false));
    }
  }
}

Minisat::vec<Minisat::Lit> const& AssignmentModifier::get() const {
  return _assignment;
}

}  // namespace core::domain
