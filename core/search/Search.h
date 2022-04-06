#ifndef EVOL_ASSIGNMENT_H
#define EVOL_ASSIGNMENT_H

#include <vector>
#include <set>
#include <map>
#include <memory>
#include <optional>
#include <algorithm>

#include "core/types.h"
#include "core/domain/VarView.h"
#include "core/domain/SearchSpace.h"
#include "util/Reference.h"
#include "util/Assert.h"

namespace core::search {

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
  [[nodiscard]] virtual lit_vec_t const& operator()() const = 0;

  /**
   * @brief Steps to the next search.
   * @return true if the state is valid and operator() can be applied.
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
   * @return the copy of this search
   */
  [[nodiscard]] virtual Search* clone() const = 0;

 public:
  static uint64_t total_size(bit_mask_t const& vars);

 protected:
  virtual void _advance() = 0;

  virtual void _reset() = 0;

 protected:
  static void _set_assignment(lit_vec_t& vec, uint64_t value);

 private:
  void _set_range();

 protected:
  uint64_t _total;
  uint64_t _done = 0;
  uint64_t _first = 0;
  uint64_t _last = 0;
};

/**
 * @brief The search implementation consisting of only one (empty) assignment.
 */
class SingleSearch final : public Search {
 public:
  SingleSearch();

  [[nodiscard]] lit_vec_t const& operator()() const override;

  [[nodiscard]] Search* clone() const override;

 private:
  void _advance() override;

  void _reset() override{};
};

MAKE_REFS(SingleSearch);

USingleSearch createSingleSearch();

}  // namespace core::search

#endif  // EVOL_ASSIGNMENT_H