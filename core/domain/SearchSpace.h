#ifndef ITMO_PARSAT_SEARCHSPACE_H
#define ITMO_PARSAT_SEARCHSPACE_H

#include <cstdint>
#include <cstddef>

namespace core::domain {

/**
 * @brief The class representing the search space for SAT assumptions.
 */
struct SearchSpace {
 public:
  /**
   * @brief Maximal number of variables allowed to be fully covered.
   */
  static constexpr uint32_t MAX_VARS_FOR_FULL_SEARCH = 63;

 public:
  /**
   * @brief If the search space can be fully covered.
   */
  bool countable_search_space = false;

  /**
   * @brief The size of the search space.
   */
  uint64_t search_space_size = 0;

  /**
   * @brief The approximation of number of visited points.
   * It is not fully accurate because the cache size is limited.
   */
  uint64_t inaccurate_visited_points = 0;

 public:
  SearchSpace() noexcept = default;

  explicit SearchSpace(uint32_t num_variables) noexcept;

  /**
   * @param num_variables the number of variables
   */
  void reset(uint32_t num_variables) noexcept;

  /**
   * @return whether there (probably) are non-visited points
   */
  [[nodiscard]] bool has_unvisited_points() const noexcept;
};

}  // namespace core::domain

#endif  // ITMO_PARSAT_SEARCHSPACE_H
