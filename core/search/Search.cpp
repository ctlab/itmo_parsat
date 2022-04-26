#include "core/search/Search.h"

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

namespace core::search {

Search::Search(uint64_t total) : _total(total) { _set_range(); }

Search::Search(std::vector<int> const& vars, uint64_t total) : Search(total) {
  _assignment.capacity((int) vars.size());
  for (int var : vars) {
    _assignment.push(Mini::mkLit(var, false));
  }
}

Search::Search(domain::VarView const& var_view, bit_mask_t const& bit_mask, uint64_t total) : Search(total) {
  for (int i = 0; i < (int) bit_mask.size(); ++i) {
    if (bit_mask[i]) {
      _assignment.push(Mini::mkLit(var_view[i], false));
    }
  }
}

lit_vec_t const& Search::operator()() const noexcept { return _assignment; }

uint64_t Search::size() const noexcept { return _total; }

bool Search::empty() const noexcept { return size() == 0; }

bool Search::operator++() {
  if (_done == _total - 1 || _total == 0) {
    return false;
  } else {
    _advance();
    ++_done;
    return true;
  }
}

void Search::_set_assignment(lit_vec_t& vec, uint64_t value) {
  for (int i = 0; i < vec.size(); ++i) {
    vec[i] = Mini::mkLit(Mini::var(vec[i]), value & (1ULL << i));
  }
}

uint64_t Search::total_size(std::vector<bool> const& vars) {
  return 1ULL << std::count(vars.begin(), vars.end(), true);
}

USearch Search::split_search(uint64_t num_split, uint64_t index) const {
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
  return USearch(result);
}

void Search::_set_range() {
  _first = 0;
  _last = _total - 1;
}

SingleSearch::SingleSearch() : Search(1) {}

void SingleSearch::_advance() {}

Search* SingleSearch::clone() const { return new SingleSearch(); }

USingleSearch createSingleSearch() { return std::make_unique<SingleSearch>(); }

}  // namespace core::search