#include "core/domain/assignment/AssignmentModifier.h"

namespace core::domain {

AssignmentModifier::AssignmentModifier(VarView const& var_view, std::vector<bool> const& vars) {
  for (int i = 0; i < (int) vars.size(); ++i) {
    if (vars[i]) {
      _assignment.push(Mini::mkLit(var_view[i], false));
    }
  }
}

AssignmentModifier::AssignmentModifier(std::vector<int> const& vars) {
  _assignment.capacity(vars.size());
  for (int var : vars) {
    _assignment.push(Mini::mkLit(var, false));
  }
}

Mini::vec<Mini::Lit> const& AssignmentModifier::get() const {
  return _assignment;
}

}  // namespace core::domain
