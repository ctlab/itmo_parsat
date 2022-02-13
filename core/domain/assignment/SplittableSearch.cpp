#include "core/domain/assignment/SplittableSearch.h"

namespace {

// clang-format off
void get_start_and_todo(
    uint64_t total,
    uint64_t num_split, uint64_t index,
    uint64_t& start, uint64_t& to_do) {
  uint64_t div = total / num_split;
  uint64_t rem = total % num_split;
  if (index < rem) {
    to_do = div + 1;
    start = index * to_do;
  } else {
    to_do = div;
    start = rem * (div + 1) + (index - rem) * div;
  }
}
// clang-format on

}  // namespace

namespace core::domain {

SplittableSearch::SplittableSearch(uint64_t total) : Search(total) {
  _set_range();
}

USplittableSearch SplittableSearch::split_search(uint64_t num_split, uint64_t index) const {
  auto* result = clone();
  uint64_t start, to_do;
  get_start_and_todo(_total, num_split, index, start, to_do);
  if (start < _total) {
    result->_first = start;
    result->_last = start + to_do - 1;
    result->_done = 0;
    result->_total = to_do;
    result->_reset();
  } else {
    result->_total = 0;
  }
  return USplittableSearch(result);
}

void SplittableSearch::_set_range() {
  _first = 0;
  _last = _total - 1;
}

}  // namespace core::domain