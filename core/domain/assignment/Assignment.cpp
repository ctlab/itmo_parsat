#include "core/domain/assignment/Assignment.h"

#include "core/util/Generator.h"

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

Assignment::Assignment(size_t total) : _total(total) {
  _set_range();
}

size_t Assignment::size() const noexcept {
  return _total;
}

bool Assignment::empty() const noexcept {
  return size() == 0;
}

bool Assignment::operator++() {
  if (_done == _total - 1 || _total == 0) {
    return false;
  } else {
    _advance();
    ++_done;
    return true;
  }
}

void Assignment::_set_assignment(Minisat::vec<Minisat::Lit>& vec, uint64_t value) {
  for (int i = 0; i < vec.size(); ++i) {
    vec[i] = Minisat::mkLit(Minisat::var(vec[i]), value & (1ULL << i));
  }
}

void Assignment::_set_range() {
  _first = 0;
  _last = _total - 1;
}

UAssignment Assignment::split_search(uint64_t num_split, uint64_t index) const {
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
  return UAssignment(result);
}

}  // namespace core::domain