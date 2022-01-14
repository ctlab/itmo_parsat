#include "core/domain/Vars.h"

namespace core::domain {

void Vars::flip(size_t pos) {
  _bit_mask[pos] = !_bit_mask[pos];
}

void Vars::resize(size_t size) {
  _bit_mask.resize(size, false);
}

std::vector<bool> const& Vars::get_mask() const noexcept {
  return _bit_mask;
}

std::vector<bool>& Vars::get_mask() noexcept {
  return _bit_mask;
}

std::vector<int> Vars::map_to_vars(VarView const& var_view) const {
  std::vector<int> vars;
  auto const& mask = _bit_mask;
  for (size_t i = 0; i < _bit_mask.size(); ++i) {
    if (mask[i]) {
      vars.push_back(var_view[(int) i]);
    }
  }
  return vars;
}

}  // namespace core::domain