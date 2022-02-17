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
#include "core/util/Reference.h"
#include "core/util/assert.h"

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

 public:
  static uint64_t total_size(std::vector<bool> const& vars);

 protected:
  virtual void _advance() = 0;

 protected:
  static void _set_assignment(Minisat::vec<Minisat::Lit>& vec, uint64_t value);

 protected:
  uint32_t _total, _done = 0;
};

class SingleSearch final : public Search {
 public:
  SingleSearch();

  [[nodiscard]] Minisat::vec<Minisat::Lit> const& operator()() const override;

 private:
  void _advance() override;
};

MAKE_REFS(SingleSearch);

USingleSearch createSingleSearch();

}  // namespace core::domain

#endif  // EVOL_ASSIGNMENT_H