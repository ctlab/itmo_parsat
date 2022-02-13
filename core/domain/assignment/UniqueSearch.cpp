#include "core/domain/assignment/UniqueSearch.h"

namespace core::domain {

UniqueSearch::UniqueSearch(VarView const& var_view, std::vector<bool> const& vars, uint64_t total)
    : AssignmentModifier(var_view, vars), SplittableSearch(total) {
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

Minisat::vec<Minisat::Lit> const& UniqueSearch::operator()() const {
  return get();
}

}  // namespace core::domain
