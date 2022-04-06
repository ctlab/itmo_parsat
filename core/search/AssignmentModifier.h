#ifndef ITMO_PARSAT_ASSIGNMENTMODIFIER_H
#define ITMO_PARSAT_ASSIGNMENTMODIFIER_H

#include <cmath>
#include <algorithm>

#include "core/types.h"
#include "core/search/Search.h"

namespace core::search {

/**
 * @brief The base class for searches that step by modifying search.
 */
class AssignmentModifier {
 public:
  /**
   * Initializes the assignment with given set of vars, all set to false.
   * @param var_view the mapping from indices to vars
   * @param vars the bit set representing variables set
   */
  AssignmentModifier(domain::VarView const& var_view, bit_mask_t const& bit_mask);

  explicit AssignmentModifier(std::vector<int> const& vars);

  /**
   * @return the current state of assignment
   */
  [[nodiscard]] lit_vec_t const& get() const;

 protected:
  lit_vec_t _assignment;
};

}  // namespace core::search

#endif  // ITMO_PARSAT_ASSIGNMENTMODIFIER_H
