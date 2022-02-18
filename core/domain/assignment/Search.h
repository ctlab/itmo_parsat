#ifndef EVOL_ASSIGNMENT_H
#define EVOL_ASSIGNMENT_H

#include <vector>
#include <set>
#include <map>
#include <memory>
#include <optional>
#include <algorithm>

#include "core/sat/minisat/minisat/mtl/Vec.h"
#include "core/sat/minisat/minisat/core/SolverTypes.h"
#include "core/domain/VarView.h"
#include "core/domain/SearchSpace.h"
#include "util/Reference.h"
#include "util/assert.h"

namespace core::domain {

class Search;
MAKE_REFS(Search);

/**
 * @brief The class representing the sequential search in assignments space.
 */
class Search {
 public:
  explicit Search(uint64_t total);

 public:
  virtual ~Search() = default;

  /**
   * @return the current assignment
   */
  [[nodiscard]] virtual Minisat::vec<Minisat::Lit> const& operator()() const = 0;

  /**
   * @brief Steps to the next search.
   * @return true if and only if state is valid and operator() can be applied.
   */
  [[nodiscard]] bool operator++();

  /**
   * @return the total number of steps.
   */
  [[nodiscard]] uint64_t size() const noexcept;

  /**
   * @return equivalent to Search::size() == 0.
   */
  [[nodiscard]] bool empty() const noexcept;

  /**
   * @brief Separates a part of the assignment.
   * @param num_split number of searches this search will be split to.
   * @param index Index of current search in range [0, num_split).
   * @return new assignment instance.
   */
  [[nodiscard]] USearch split_search(uint64_t num_split, uint64_t index) const;

  /**
   * @brief Clones the current search.
   * @return the copy of this search
   */
  [[nodiscard]] virtual Search* clone() const = 0;

 public:
  static uint64_t total_size(std::vector<bool> const& vars);

 protected:
  virtual void _advance() = 0;

  virtual void _reset() = 0;

 protected:
  static void _set_assignment(Minisat::vec<Minisat::Lit>& vec, uint64_t value);

 private:
  void _set_range();

 protected:
  uint64_t _total;
  uint64_t _done = 0;
  uint64_t _first = 0;
  uint64_t _last = 0;
};

class SingleSearch final : public Search {
 public:
  SingleSearch();

  [[nodiscard]] Minisat::vec<Minisat::Lit> const& operator()() const override;

  [[nodiscard]] Search* clone() const override;

 private:
  void _advance() override;

  void _reset() override {};
};

MAKE_REFS(SingleSearch);

USingleSearch createSingleSearch();

}  // namespace core::domain

#endif  // EVOL_ASSIGNMENT_H