#ifndef ITMO_PARSAT_MODIFYINGSEARCH_H
#define ITMO_PARSAT_MODIFYINGSEARCH_H

#include <cmath>
#include <algorithm>

#include "core/domain/assignment/Search.h"

namespace core::domain {

/**
 * @brief The base class for searches that step by modifying assignment.
 */
class ModifyingSearch : public Search {
 public:
  ModifyingSearch(VarView const& var_view, std::vector<bool> const& vars, size_t total);

  ModifyingSearch(VarView const& var_view, std::vector<bool> const& vars);

  Minisat::vec<Minisat::Lit> const& operator()() const override;

 protected:
  Minisat::vec<Minisat::Lit> _assignment;
};

}  // namespace core::domain

#endif  // ITMO_PARSAT_MODIFYINGSEARCH_H
