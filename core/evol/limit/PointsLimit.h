#ifndef ITMO_PARSAT_POINTSLIMIT_H
#define ITMO_PARSAT_POINTSLIMIT_H

#include "core/evol/limit/Limit.h"

namespace ea::limit {

/**
 * @brief This limit controls the execution by limiting the number of total (inaccurate!) points
 * visited.
 */
class PointsLimit : public Limit {
 public:
  explicit PointsLimit(PointsLimitConfig const& config);

 protected:
  /**
   * @return true iff the number of (inaccurate) visited points is less than the specified limit
   */
  bool _proceed(ea::algorithm::Algorithm& algorithm) override;

 private:
  uint32_t _max_visited;
};

}  // namespace ea::limit

#endif  // ITMO_PARSAT_POINTSLIMIT_H
