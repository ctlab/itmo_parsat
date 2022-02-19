#include "core/domain/assignment/RandomSearch.h"
#include "core/domain/SearchSpace.h"
#include "UniqueSearch.h"

namespace {

void set_random(Minisat::vec<Minisat::Lit>& vec) {
  for (int i = 0; i < vec.size(); ++i) {
    vec[i] = Minisat::mkLit(Minisat::var(vec[i]), ::core::random::sample<int>(0, 1));
  }
}

}  // namespace

namespace core::domain {

RandomSearch::RandomSearch(VarView const& var_view, std::vector<bool> const& vars, uint64_t total)
    : AssignmentModifier(var_view, vars), Search(total) {
  set_random(_assignment);
}

void RandomSearch::_advance() {
  set_random(_assignment);
}

void RandomSearch::_reset() {
  _advance();
}

RandomSearch* RandomSearch::clone() const {
  return new RandomSearch(*this);
}

Minisat::vec<Minisat::Lit> const& RandomSearch::operator()() const {
  return get();
}

USearch createRandomSearch(VarView const& var_view, std::vector<bool> const& vars, uint64_t total) {
  uint32_t num_set = std::count(vars.begin(), vars.end(), true);
  if (num_set <= SearchSpace::MAX_VARS_FOR_FULL_SEARCH) {
    return USearch(new UniqueSearch(var_view, vars, total));
  } else {
    return USearch(new RandomSearch(var_view, vars, total));
  }
}

}  // namespace core::domain
