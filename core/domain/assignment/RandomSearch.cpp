#include "core/domain/assignment/RandomSearch.h"

namespace {

void set_random(Minisat::vec<Minisat::Lit>& vec) {
  for (int i = 0; i < vec.size(); ++i) {
    vec[i] = Minisat::mkLit(Minisat::var(vec[i]), ::core::random::sample<int>(0, 1));
  }
}

}  // namespace

namespace core::domain {

RandomSearch::RandomSearch(VarView const& var_view, std::vector<bool> const& vars, uint64_t total)
    : ModifyingSearch(var_view, vars, total) {
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

USearch createRandomSearch(VarView const& var_view, std::vector<bool> const& vars, uint64_t total) {
  size_t num_set = std::count(vars.begin(), vars.end(), true);
  if (num_set <= Search::MAX_VARS_FULL_SEARCH) {
    return USearch(new UniqueSearch(var_view, vars, total));
  } else {
    return USearch(new RandomSearch(var_view, vars, total));
  }
}

}  // namespace core::domain
