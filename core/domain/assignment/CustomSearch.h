#ifndef ITMO_PARSAT_CUSTOMSEARCH_H
#define ITMO_PARSAT_CUSTOMSEARCH_H

#include <vector>

#include "core/domain/assignment/Assignment.h"

namespace core::domain {

class CustomSearch : public Assignment {
 public:
  CustomSearch(std::vector<Minisat::vec<Minisat::Lit>> const& assignments);

  Minisat::vec<Minisat::Lit> const& operator()() const override;

 protected:
  void _advance() override;

  void _reset() override;

  [[nodiscard]] CustomSearch* clone() const override;

 private:
  size_t _idx = 0;
  std::vector<Minisat::vec<Minisat::Lit>> _assignments;
};

UAssignment createCustomSearch(std::vector<Minisat::vec<Minisat::Lit>> const& assignments);

}  // namespace core::domain

#endif  // ITMO_PARSAT_CUSTOMSEARCH_H
