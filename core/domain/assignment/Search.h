#ifndef EVOL_ASSIGNMENT_H
#define EVOL_ASSIGNMENT_H

#include <vector>
#include <set>
#include <map>
#include <memory>
#include <optional>

#include "core/sat/minisat/minisat/mtl/Vec.h"
#include "core/sat/minisat/minisat/core/SolverTypes.h"
#include "core/domain/VarView.h"
#include "core/util/assert.h"

namespace core::domain {

class Search;

using USearch = std::unique_ptr<Search>;

using RSearch = std::shared_ptr<Search>;

/**
 * @brief The class representing the sequential search in assignments space.
 */
class Search {
 public:
  static constexpr int MAX_VARS_FULL_SEARCH = 63;

  explicit Search(size_t total);

 public:
  virtual ~Search() = default;

  /**
   * @return the current assignment
   */
  virtual Minisat::vec<Minisat::Lit> const& operator()() const = 0;

  /**
   * @brief Separates a part of the assignment.
   * @param num_split number of searches this search will be split to.
   * @param index Index of current search in range [0, num_split).
   * @return new assignment instance.
   */
  [[nodiscard]] USearch split_search(uint64_t num_split, uint64_t index) const;

  /**
   * @brief Steps to the next search.
   * @return true if and only if state is valid and operator() can be applied.
   */
  [[nodiscard]] bool operator++();

  /**
   * @return the total number of steps.
   */
  [[nodiscard]] size_t size() const noexcept;

  /**
   * @return equivalent to Search::size() == 0.
   */
  [[nodiscard]] bool empty() const noexcept;

 protected:
  virtual void _advance() = 0;

  virtual void _reset() = 0;

  [[nodiscard]] virtual Search* clone() const = 0;

 protected:
  static void _set_assignment(Minisat::vec<Minisat::Lit>& vec, uint64_t value);

 private:
  void _set_range();

 protected:
  uint32_t _total, _done = 0;
  uint32_t _first, _last;
};

}  // namespace core::domain

#endif  // EVOL_ASSIGNMENT_H