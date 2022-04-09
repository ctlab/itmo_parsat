#include "Shareable.h"

namespace core::sat::sharing {

void concat(SharingUnit& a, SharingUnit const& b) {
  IPS_VERIFY_S(
      a.index() == b.index(),
      "Trying to concat different types of sharing units.");
  std::visit(
      overloaded{
          [&b](SolverList& sl) {
            auto const& bsl = std::get<SolverList>(b);
            sl.insert(sl.end(), bsl.begin(), bsl.end());
          },
          [&b](SolverBlockList& sbl) {
            auto const& bsbl = std::get<SolverBlockList>(b);
            sbl.insert(sbl.end(), bsbl.begin(), bsbl.end());
          }},
      a);
}

}  // namespace core::sat::sharing
