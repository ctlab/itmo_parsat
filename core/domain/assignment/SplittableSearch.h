#ifndef ITMO_PARSAT_SPLITTABLESEARCH_H
#define ITMO_PARSAT_SPLITTABLESEARCH_H

#include "core/domain/assignment/Search.h"

namespace core::domain {

class SplittableSearch;
MAKE_REFS(SplittableSearch);

class SplittableSearch : public Search {
 public:
  explicit SplittableSearch(uint64_t total);

  /**
   * @brief Separates a part of the assignment.
   * @param num_split number of searches this search will be split to.
   * @param index Index of current search in range [0, num_split).
   * @return new assignment instance.
   */
  [[nodiscard]] USplittableSearch split_search(uint64_t num_split, uint64_t index) const;

  /**
   * @brief Clones the current search.
   * @return the copy of this search
   */
  [[nodiscard]] virtual SplittableSearch* clone() const = 0;

 protected:
  virtual void _reset() = 0;

 protected:
  uint64_t _first = 0;
  uint64_t _last = 0;

 private:
  void _set_range();
};

}  // namespace core::domain

#endif  // ITMO_PARSAT_SPLITTABLESEARCH_H
