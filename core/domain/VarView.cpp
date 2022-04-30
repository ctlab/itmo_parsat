#include "core/domain/VarView.h"

namespace core::domain {

void VarView::map_var(int index, int var) {
  if (_map.size() <= index) {
    _map.resize(1 + index);
  }
  _map[index] = var;
}

int VarView::operator[](int index) const noexcept { return _map[index]; }

uint32_t VarView::size() const noexcept { return _map.size(); }

std::vector<int> const& VarView::get_map() const noexcept { return _map; }

}  // namespace core::domain
