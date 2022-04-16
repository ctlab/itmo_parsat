#include "core/search/CartesianSearch.h"

#include <memory>

namespace core::search {

CartesianSearch::CartesianSearch(std::shared_ptr<cartesian_set_t> cartesian_set)
    : Search(std::accumulate(
          cartesian_set->begin(), cartesian_set->end(), uint32_t(1),
          [](uint32_t size, auto const& v) { return size * v.size(); }))
    , _r_cartesian_set(std::move(cartesian_set)) {
  _assignment.growTo((int) std::accumulate(
      _r_cartesian_set->begin(), _r_cartesian_set->end(), uint32_t(0),
      [](uint32_t size, auto const& v) { return size + v.front().size(); }));
  _indices.resize(_r_cartesian_set->size(), 0);
  _set_cur(0);
}

CartesianSearch::CartesianSearch(cartesian_set_t&& cartesian)
    : CartesianSearch(std::make_shared<cartesian_set_t>(std::move(cartesian))) {}

CartesianSearch::CartesianSearch(cartesian_set_t const& cartesian)
    : CartesianSearch(std::make_shared<cartesian_set_t>(cartesian)) {}

Search* CartesianSearch::clone() const {
  return new CartesianSearch(_r_cartesian_set);
}

void CartesianSearch::_reset() {
  auto& cartesian = *_r_cartesian_set;
  size_t tmp_first = _first;
  for (size_t i = _indices.size(); i-- > 0;) {
    _indices[i] = tmp_first % cartesian[i].size();
    tmp_first /= cartesian[i].size();
  }
  _set_cur(0);
}

void CartesianSearch::_set_cur(uint32_t from) {
  auto& cartesian = *_r_cartesian_set;
  uint32_t offset = 0;
  for (uint32_t i = 0; i < from; ++i) { offset += cartesian[i][_indices[i]].size(); }
  for (size_t i = from; i < _indices.size(); ++i) {
    uint32_t cur_index = _indices[i];
    auto const& assignment = cartesian[i][cur_index];
    uint32_t cur_size = assignment.size();
    for (uint32_t j = 0; j < cur_size; ++j) { _assignment[(int) (offset + j)] = assignment[j]; }
    offset += cur_size;
  }
}

void CartesianSearch::_advance() {
  auto& cartesian = *_r_cartesian_set;
  int index = (int) _indices.size() - 1;
  while (index >= 0 && _indices[index] + 1 == cartesian[index].size()) {
    _indices[index] = 0;
    --index;
  }
  if (index >= 0) { ++_indices[index]; }
  _set_cur(std::max(index, 0));
}

UCartesianSearch createCartesianSearch(CartesianSearch::cartesian_set_t&& cartesian) {
  return std::make_unique<CartesianSearch>(std::move(cartesian));
}

UCartesianSearch createCartesianSearch(CartesianSearch::cartesian_set_t const& cartesian) {
  return std::make_unique<CartesianSearch>(cartesian);
}

}  // namespace core::search
