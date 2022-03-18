#include "core/domain/assignment/FullSearch.h"

#include <iostream>
#include <memory>

namespace {

void next_assignment(Mini::vec<Mini::Lit>& vec) {
  int pos = 0;
  while (pos < vec.size() && sign(vec[pos])) {
    vec[pos] = ~vec[pos];
    ++pos;
  }
  if (pos < vec.size()) {
    vec[pos] = Mini::mkLit(Mini::var(vec[pos]), true);
  }
}

}  // namespace

namespace core::domain {

FullSearch::FullSearch(VarView const& var_view, std::vector<bool> const& vars)
    : AssignmentModifier(var_view, vars), Search(Search::total_size(vars)) {}

FullSearch::FullSearch(std::vector<int> const& vars)
    : AssignmentModifier(vars), Search(1ULL << vars.size()) {}

void FullSearch::_advance() {
  next_assignment(_assignment);
}

FullSearch* FullSearch::clone() const {
  return new FullSearch(*this);
}

void FullSearch::_reset() {
  _set_assignment(_assignment, _first);
}

Mini::vec<Mini::Lit> const& FullSearch::operator()() const {
  return get();
}

UFullSearch createFullSearch(
    VarView const& var_view, std::vector<bool> const& vars) {
  return std::make_unique<FullSearch>(var_view, vars);
}

UFullSearch createFullSearch(std::vector<int> const& vars) {
  return std::make_unique<FullSearch>(vars);
}

}  // namespace core::domain
