#ifndef ITMO_PARSAT_SEARCHSPACE_H
#define ITMO_PARSAT_SEARCHSPACE_H

#include <cstdint>
#include <cstddef>

namespace core::domain {

struct SearchSpace {
 public:
  static constexpr uint32_t MAX_VARS_FOR_FULL_SEARCH = 63;

 public:
  bool countable_search_space = false;
  uint64_t search_space_size = 0;
  uint64_t inaccurate_visited_points = 0;

 public:
  SearchSpace() noexcept = default;

  explicit SearchSpace(uint32_t num_variables) noexcept;

  void reset(uint32_t num_variables) noexcept;

  [[nodiscard]] bool has_unvisited_points() const noexcept;
};

}  // namespace core::domain

#endif  // ITMO_PARSAT_SEARCHSPACE_H
