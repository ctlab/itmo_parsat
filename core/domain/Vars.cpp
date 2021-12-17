#include "core/domain/Vars.h"

namespace core::domain {

void Vars::flip(size_t pos) {
  bit_mask_[pos] = !bit_mask_[pos];
}

void Vars::resize(size_t size) {
  bit_mask_.resize(size, false);
}

std::vector<bool> const& Vars::get_mask() const noexcept {
  return bit_mask_;
}

std::vector<bool>& Vars::get_mask() noexcept {
  return bit_mask_;
}

std::vector<int> Vars::map_to_vars(VarView const& var_view) const {
  std::vector<int> vars;
  auto const& mask = bit_mask_;
  for (size_t i = 0; i < bit_mask_.size(); ++i) {
    if (mask[i]) {
      vars.push_back(var_view[(int) i]);
    }
  }
  return vars;
}

}  // namespace core::domain