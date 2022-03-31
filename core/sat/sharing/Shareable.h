#ifndef ITMO_PARSAT_SHAREABLE_H
#define ITMO_PARSAT_SHAREABLE_H

#include <vector>
#include <memory>
#include <variant>

#include "core/sat/native/painless/painless-src/sharing/Sharer.h"
#include "core/sat/native/painless/painless-src/sharing/HordeSatSharing.h"
#include "util/Assert.h"
#include "util/visit.h"

namespace core::sat::sharing {

using SolverList = std::vector<SolverInterface*>;

using SolverBlock = std::tuple<  //
    SolverList,                  // solvers
    SolverInterface*             // reducer
    >;

using SolverBlockList = std::vector<SolverBlock>;

using SharingUnit = std::variant<  //
    SolverList,                    // solvers with no reducer
    SolverBlockList                // solvers + reducer
    >;

void concat(SharingUnit& a, SharingUnit const& b);

class Shareable {
 public:
  /**
   * @return the sharing unit to share with
   */
  virtual SharingUnit sharing_unit() noexcept = 0;
};

}  // namespace core::sat::sharing

#endif  // ITMO_PARSAT_SHAREABLE_H
