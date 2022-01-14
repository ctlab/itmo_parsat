#include "core/domain/assignment/CustomSearch.h"

namespace core::domain {

CustomSearch::CustomSearch(std::vector<Minisat::vec<Minisat::Lit>> const& assignments)
    : Search(assignments.size()), _assignments(assignments) {}

Minisat::vec<Minisat::Lit> const& CustomSearch::operator()() const {
  return _assignments[_idx];
}

void CustomSearch::_advance() {
  ++_idx;
}

void CustomSearch::_reset() {
  _idx = _first;
}

CustomSearch* CustomSearch::clone() const {
  return new CustomSearch(*this);
}

USearch createCustomSearch(std::vector<Minisat::vec<Minisat::Lit>> const& assignments) {
  return USearch(new CustomSearch(assignments));
}

}  // namespace core::domain
