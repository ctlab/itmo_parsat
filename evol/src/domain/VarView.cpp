#include "evol/include/domain/VarView.h"

namespace ea::domain {

void VarView::map_var(int index, int var) {
  map_[index] = var;
}

int VarView::operator[](int index) const noexcept {
  return map_.at(index);
}

size_t VarView::size() const noexcept {
  return map_.size();
}

}  // namespace ea::domain