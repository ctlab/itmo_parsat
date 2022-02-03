#include "core/domain/assignment/ModifyingSearch.h"

#include <iostream>

namespace core::domain {

ModifyingSearch::ModifyingSearch(
    VarView const& var_view, std::vector<bool> const& vars, uint64_t total)
    : Search(total) {
  for (int i = 0; i < (int) vars.size(); ++i) {
    if (vars[i]) {
      _assignment.push(Minisat::mkLit(var_view[i], false));
    }
  }
}

ModifyingSearch::ModifyingSearch(VarView const& var_view, std::vector<bool> const& vars)
    : ModifyingSearch(var_view, vars, 1ULL << std::count(vars.begin(), vars.end(), true)) {}

Minisat::vec<Minisat::Lit> const& ModifyingSearch::operator()() const {
  return _assignment;
}

}  // namespace core::domain
