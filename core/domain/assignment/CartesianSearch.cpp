#include "core/domain/assignment/CartesianSearch.h"

namespace core::domain {

CartesianSearch::CartesianSearch(std::vector<std::vector<std::vector<Minisat::Lit>>>&& cartesian)
    // clang-format off
  : Search(std::accumulate(cartesian.begin(), cartesian.end(), uint32_t(1), [] (uint32_t size, auto const& v) {
      return size * v.size();
    }))
  , _assignment((int) std::accumulate(cartesian.begin(), cartesian.end(), uint32_t(0), [] (uint32_t size, auto const& v) {
      return size + v.front().size();
    }))
  , _cartesian(std::move(cartesian)) {
  // clang-format on
  _indices.resize(_cartesian.size(), 0);
  _set_cur(0);
}

Minisat::vec<Minisat::Lit> const& CartesianSearch::operator()() const {
  return _assignment;
}

void CartesianSearch::_set_cur(uint32_t from) {
  uint32_t offset = 0;
  for (uint32_t i = 0; i < from; ++i) {
    offset += _cartesian[i][_indices[i]].size();
  }
  for (size_t i = from; i < _indices.size(); ++i) {
    uint32_t cur_index = _indices[i];
    auto const& assignment = _cartesian[i][cur_index];
    uint32_t cur_size = assignment.size();
    for (uint32_t j = 0; j < cur_size; ++j) {
      _assignment[(int) (offset + j)] = assignment[j];
    }
    offset += cur_size;
  }
}

void CartesianSearch::_advance() {
  int index = (int) _indices.size() - 1;
  while (index >= 0 && _indices[index] + 1 == _cartesian[index].size()) {
    _indices[index] = 0;
    --index;
  }
  if (index >= 0) {
    ++_indices[index];
  }
  _set_cur(std::max(index, 0));
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
