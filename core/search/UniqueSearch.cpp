#include "core/search/UniqueSearch.h"

namespace core::search {

UniqueSearch::UniqueSearch(
    domain::VarView const& var_view, bit_mask_t const& vars, uint64_t total)
    : AssignmentModifier(var_view, vars), Search(total) {
  _advance_us();
}

UniqueSearch::UniqueSearch(std::vector<int> const& vars, uint64_t total)
    : AssignmentModifier(vars), Search(total) {
  _advance_us();
}

void UniqueSearch::_advance() {
  _advance_us();
}

void UniqueSearch::_advance_us() {
  uint64_t sample = _first;
  while (visited_.count(sample)) {
    sample = util::random::sample<uint64_t>(_first + 1, _last);
  }
  visited_.insert(sample);
  _set_assignment(_assignment, sample);
}

UniqueSearch* UniqueSearch::clone() const {
  return new UniqueSearch(*this);
}

void UniqueSearch::_reset() {
  visited_.clear();
  _advance();
}

lit_vec_t const& UniqueSearch::operator()() const {
  return get();
}

}  // namespace core::search
