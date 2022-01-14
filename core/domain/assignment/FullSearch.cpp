#include "core/domain/assignment/FullSearch.h"

#include <iostream>

namespace {

void next_assignment(Minisat::vec<Minisat::Lit>& vec) {
  int pos = 0;
  while (pos < vec.size() && sign(vec[pos])) {
    vec[pos] = ~vec[pos];
    ++pos;
  }
  if (pos < vec.size()) {
    vec[pos] = Minisat::mkLit(Minisat::var(vec[pos]), true);
  }
}

}  // namespace

namespace core::domain {

FullSearch::FullSearch(VarView const& var_view, std::vector<bool> const& vars)
    : ModifyingSearch(var_view, vars) {}

void FullSearch::_advance() {
  next_assignment(_assignment);
}

FullSearch* FullSearch::clone() const {
  return new FullSearch(*this);
}

void FullSearch::_reset() {
  _set_assignment(_assignment, _first);
}

USearch createFullSearch(VarView const& var_view, std::vector<bool> const& vars) {
  return USearch(new FullSearch(var_view, vars));
}

}  // namespace core::domain
