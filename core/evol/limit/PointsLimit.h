#ifndef ITMO_PARSAT_POINTSLIMIT_H
#define ITMO_PARSAT_POINTSLIMIT_H

#include "core/evol/limit/Limit.h"

namespace ea::limit {

class PointsLimit : public Limit {
 public:
  /**
   * This limit controls the execution by limiting the number of total (inaccurate!) points visited.
   */
  explicit PointsLimit(PointsLimitConfig const& config);

  /**
   * @return true iff the nubmer of (inaccurate) visited points is less than the specified limit
   */
  bool proceed(ea::algorithm::Algorithm& algorithm) override;

 private:
  uint32_t _max_visited;
};

}  // namespace ea::limit

#endif  // ITMO_PARSAT_POINTSLIMIT_H
