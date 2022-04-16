#include "core/search/FullSearch.h"

namespace {

void next_assignment(core::lit_vec_t& vec) {
  int pos = 0;
  while (pos < vec.size() && sign(vec[pos])) {
    vec[pos] = ~vec[pos];
    ++pos;
  }
  if (pos < vec.size()) { vec[pos] = Mini::mkLit(Mini::var(vec[pos]), true); }
}

}  // namespace

namespace core::search {

FullSearch::FullSearch(domain::VarView const& var_view, bit_mask_t const& bit_mask)
    : Search(var_view, bit_mask, Search::total_size(bit_mask)) {}

FullSearch::FullSearch(std::vector<int> const& vars) : Search(vars, 1ULL << vars.size()) {}

void FullSearch::_advance() {
  next_assignment(_assignment);
}

FullSearch* FullSearch::clone() const {
  return new FullSearch(*this);
}

void FullSearch::_reset() {
  _set_assignment(_assignment, _first);
}

UFullSearch createFullSearch(domain::VarView const& var_view, bit_mask_t const& bit_mask) {
  return std::make_unique<FullSearch>(var_view, bit_mask);
}

UFullSearch createFullSearch(std::vector<int> const& vars) {
  return std::make_unique<FullSearch>(vars);
}

}  // namespace core::search
