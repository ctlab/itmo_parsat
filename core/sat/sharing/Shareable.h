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

/**
 * @brief The list of painless-based solvers capable of clause sharing.
 */
using SolverList = std::vector<painless::SolverInterface*>;

/**
 * @brief The block of sharing solvers (with or without a Reducer).
 */
using SolverBlock = std::tuple<  //
    SolverList,                  // solvers
    painless::SolverInterface*   // reducer
    >;

/**
 * @brief The list of SolverBlocks
 */
using SolverBlockList = std::vector<SolverBlock>;

using SharingUnit = std::variant<  //
    SolverList,                    // solvers with no reducer
    SolverBlockList                // solvers + reducer
    >;

/**
 * @brief Concatenates two SharingUnit instances into the first one.
 * @param a the first and the output SharingUnit
 * @param b the second SharingUnit
 */
void concat(SharingUnit& a, SharingUnit const& b);

/**
 * @brief The interface of methods that are able to share clauses.
 */
class Shareable {
 public:
  /**
   * @return the SharingUnit to share clauses with
   */
  virtual SharingUnit sharing_unit() noexcept = 0;
};

}  // namespace core::sat::sharing

#endif  // ITMO_PARSAT_SHAREABLE_H
