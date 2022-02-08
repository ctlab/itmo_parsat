#include "core/domain/assignment/CartesianSearch.h"

namespace core::domain {

CartesianSearch::CartesianSearch(std::vector<std::vector<std::vector<Minisat::Lit>>>&& cartesian)
    // clang-format off
  : Search(std::accumulate(cartesian.begin(), cartesian.end(), uint32_t(1), [] (uint32_t size, auto const& v) {
      return size * v.size();
    }))
  , _assignment(std::accumulate(cartesian.begin(), cartesian.end(), uint32_t(0), [] (uint32_t size, auto const& v) {
      return size + v.front().size();
    }))
  , _cartesian(std::move(cartesian)) {
  // clang-format on
  _indices.resize(_cartesian.size(), 0);
  _sizes.resize(_cartesian.size());
  for (size_t i = 0; i < _sizes.size(); ++i) {
    _sizes[i] = (int) _cartesian[i].size();
  }
  _set_cur();
}

Minisat::vec<Minisat::Lit> const& CartesianSearch::operator()() const {
  return _assignment;
}

void CartesianSearch::_set_cur() {
  uint32_t offset = 0;
  for (size_t i = 0; i < _sizes.size(); ++i) {
    size_t cur_index = _indices[i];
    size_t cur_size = _cartesian[i][cur_index].size();
    for (size_t j = 0; j < cur_size; ++j) {
      _assignment[offset + j] = _cartesian[i][cur_index][j];
    }
    offset += cur_size;
  }
}

void CartesianSearch::_advance() {
  int index = (int) _indices.size() - 1;
  while (index >= 0 && _indices[index] + 1 == _sizes[index]) {
    _indices[index] = 0;
    --index;
  }
  if (index >= 0) {
    ++_indices[index];
  }
  _set_cur();
}

void CartesianSearch::_reset() {
  IPS_ERROR("Reset/Split is not supported for CartesianSearch");
}

CartesianSearch* CartesianSearch::clone() const {
  return new CartesianSearch(*this);
}

USearch createCartesianSearch(std::vector<std::vector<std::vector<Minisat::Lit>>>&& cartesian) {
  return USearch(new CartesianSearch(std::move(cartesian)));
}

}  // namespace core::domain
