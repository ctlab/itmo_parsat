#ifndef ITMO_PARSAT_UNIQUESEARCH_H
#define ITMO_PARSAT_UNIQUESEARCH_H

#include "unordered_set"

#include "core/domain/assignment/AssignmentModifier.h"
#include "core/domain/assignment/Search.h"
#include "util/Random.h"

namespace core::domain {

/**
 * @brief The class used to perform unique random search. Used for small sets of
 * variables.
 */
class UniqueSearch : AssignmentModifier, public Search {
 public:
  explicit UniqueSearch(
      VarView const& var_view, bit_mask_t const& vars, uint64_t total);

  explicit UniqueSearch(std::vector<int> const& vars, uint64_t total);

  [[nodiscard]] lit_vec_t const& operator()() const override;

 private:
  std::unordered_set<uint64_t> visited_;

 protected:
  void _advance() override;

  void _reset() override;

  [[nodiscard]] UniqueSearch* clone() const override;

  void _advance_us();
};

MAKE_REFS(UniqueSearch);

}  // namespace core::domain

#endif  // ITMO_PARSAT_UNIQUESEARCH_H
