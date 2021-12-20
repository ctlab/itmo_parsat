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

class Assignment;

using UAssignment = std::unique_ptr<Assignment>;

using RAssignment = std::shared_ptr<Assignment>;

class Assignment {
 public:
  static constexpr int MAX_VARS_FULL_SEARCH = 63;

  explicit Assignment(size_t total);

 public:
  virtual ~Assignment() = default;

  /**
   * @return the current assignment
   */
  virtual Minisat::vec<Minisat::Lit> const& operator()() const = 0;

  /**
   * Gets part of the assignment.
   * @param num_split Number of searches this search will be split to
   * @param index Index of current search in range [0, num_split)
   * @return new search
   */
  [[nodiscard]] UAssignment split_search(uint64_t num_split, uint64_t index) const;

  /**
   * Steps to the next search.
   *
   * Intended to be used as follows:
   *
   * // assignment is defined
   * do {
   *   // use assignment()
   * } while (++assignment);
   *
   * @return true iff state is valid and operator() can be applied
   */
  [[nodiscard]] bool operator++();

  [[nodiscard]] size_t size() const noexcept;

  [[nodiscard]] bool empty() const noexcept;

 protected:
  virtual void _advance() = 0;

  virtual void _reset() = 0;

  [[nodiscard]] virtual Assignment* clone() const = 0;

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