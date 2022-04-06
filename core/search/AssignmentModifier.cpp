#include "core/search/AssignmentModifier.h"

namespace core::search {

AssignmentModifier::AssignmentModifier(
    domain::VarView const& var_view, bit_mask_t const& bit_mask) {
  for (int i = 0; i < (int) bit_mask.size(); ++i) {
    if (bit_mask[i]) {
      _assignment.push(Mini::mkLit(var_view[i], false));
    }
  }
}

AssignmentModifier::AssignmentModifier(std::vector<int> const& vars) {
  _assignment.capacity((int) vars.size());
  for (int var : vars) {
    _assignment.push(Mini::mkLit(var, false));
  }
}

lit_vec_t const& AssignmentModifier::get() const {
  return _assignment;
}

}  // namespace core::search
