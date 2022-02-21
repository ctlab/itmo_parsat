#ifndef ITMO_PARSAT_UNIQUESEARCH_H
#define ITMO_PARSAT_UNIQUESEARCH_H

#include "unordered_set"

#include "core/domain/assignment/AssignmentModifier.h"
#include "core/domain/assignment/Search.h"
#include "util/Random.h"

namespace core::domain {

class FullSearch;
class UniqueSearch;
MAKE_REFS(UniqueSearch);

/**
 * @brief The class used to perform unique random search. Used for small sets of variables.
 */
class UniqueSearch : AssignmentModifier, public Search {
  friend std::unique_ptr<FullSearch> createFullSearch(VarView const&, std::vector<bool> const&);

  friend USearch createRandomSearch(VarView const&, std::vector<bool> const&, uint64_t);

 public:
  explicit UniqueSearch(VarView const& var_view, std::vector<bool> const& vars, uint64_t total);

  [[nodiscard]] Minisat::vec<Minisat::Lit> const& operator()() const override;

 private:
  std::unordered_set<uint64_t> visited_;

 protected:
  void _advance() override;

  void _reset() override;

  [[nodiscard]] UniqueSearch* clone() const override;

  void _advance_us();
};

}  // namespace core::domain

#endif  // ITMO_PARSAT_UNIQUESEARCH_H
