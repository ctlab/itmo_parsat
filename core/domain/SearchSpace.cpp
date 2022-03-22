#include "core/domain/SearchSpace.h"

namespace core::domain {

SearchSpace::SearchSpace(uint32_t num_variables) noexcept {
  reset(num_variables);
}

void SearchSpace::reset(uint32_t num_variables) noexcept {
  inaccurate_visited_points = 0;
  countable_search_space = num_variables <= MAX_VARS_FOR_FULL_SEARCH;
  if (countable_search_space) {
    search_space_size = uint64_t(1) << num_variables;
  }
}

bool SearchSpace::has_unvisited_points() const noexcept {
  return !countable_search_space ||
         inaccurate_visited_points < search_space_size;
}

}  // namespace core::domain