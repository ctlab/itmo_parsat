#ifndef ITMO_PARSAT_UNIQUESEARCH_H
#define ITMO_PARSAT_UNIQUESEARCH_H

#include "core/domain/assignment/ModifierAssignment.h"
#include "core/util/Generator.h"

namespace core::domain {

class UniqueSearch : public ModifierAssignment {
  friend UAssignment createFullSearch(VarView const&, std::vector<bool> const&);

  friend UAssignment createRandomSearch(VarView const&, std::vector<bool> const&, uint64_t);

 public:
  explicit UniqueSearch(VarView const& var_view, std::vector<bool> const& vars, uint64_t total);

  explicit UniqueSearch(VarView const& var_view, std::vector<bool> const& vars);

 private:
  std::set<uint64_t> visited_;

 protected:
  void _advance() override;

  void _reset() override;

  [[nodiscard]] UniqueSearch* clone() const override;

  void _advance_us();
};

}  // namespace core::domain

#endif  // ITMO_PARSAT_UNIQUESEARCH_H
