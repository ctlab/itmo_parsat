#include "core/domain/assignment/UniqueSearch.h"

namespace core::domain {

UniqueSearch::UniqueSearch(const VarView& var_view, const std::vector<bool>& vars, uint64_t total)
    : ModifyingSearch(var_view, vars, total) {
  _advance_us();
}

UniqueSearch::UniqueSearch(const VarView& var_view, const std::vector<bool>& vars)
    : ModifyingSearch(var_view, vars) {
  _advance_us();
}

void UniqueSearch::_advance() {
  _advance_us();
}

void UniqueSearch::_advance_us() {
  uint64_t sample = _first;
  while (visited_.count(sample)) {
    sample = random::sample<uint64_t>(_first + 1, _last);
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

}  // namespace core::domain
