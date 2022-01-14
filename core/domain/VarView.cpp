#include "core/domain/VarView.h"

namespace core::domain {

void VarView::map_var(int index, int var) {
  _map[index] = var;
}

int VarView::operator[](int index) const noexcept {
  return _map.at(index);
}

size_t VarView::size() const noexcept {
  return _map.size();
}

}  // namespace core::domain