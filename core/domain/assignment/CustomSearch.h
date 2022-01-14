#ifndef ITMO_PARSAT_CUSTOMSEARCH_H
#define ITMO_PARSAT_CUSTOMSEARCH_H

#include <vector>

#include "core/domain/assignment/Search.h"

namespace core::domain {

/**
 * @brief The class used to search through the custom set of assignments.
 */
class CustomSearch : public Search {
 public:
  /**
   * @param assignments the set of assignments to be searched through.
   */
  CustomSearch(std::vector<Minisat::vec<Minisat::Lit>> const& assignments);

  /**
   * @see Search::operator()()
   */
  Minisat::vec<Minisat::Lit> const& operator()() const override;

 protected:
  void _advance() override;

  void _reset() override;

  [[nodiscard]] CustomSearch* clone() const override;

 private:
  size_t _idx = 0;
  std::vector<Minisat::vec<Minisat::Lit>> _assignments;
};

USearch createCustomSearch(std::vector<Minisat::vec<Minisat::Lit>> const& assignments);

}  // namespace core::domain

#endif  // ITMO_PARSAT_CUSTOMSEARCH_H
