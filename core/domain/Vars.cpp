#include "core/domain/Vars.h"

namespace core::domain {

void Vars::flip(uint32_t pos) { _bit_mask[pos] = !_bit_mask[pos]; }

void Vars::resize(uint32_t size) { _bit_mask.resize(size, false); }

core::bit_mask_t const& Vars::get_mask() const noexcept { return _bit_mask; }

core::bit_mask_t& Vars::get_mask() noexcept { return _bit_mask; }

std::vector<int> Vars::map_to_vars(VarView const& var_view) const {
  std::vector<int> vars;
  auto const& mask = _bit_mask;
  for (uint32_t i = 0; i < _bit_mask.size(); ++i) {
    if (mask[i]) {
      vars.push_back(var_view[(int) i]);
    }
  }
  return vars;
}

uint32_t Vars::size() const noexcept { return std::count(_bit_mask.begin(), _bit_mask.end(), true); }

}  // namespace core::domain