#include "core/evol/limit/PointsLimit.h"

#include "core/evol/algorithm/Algorithm.h"

namespace ea::limit {

PointsLimit::PointsLimit(PointsLimitConfig const& config)
    : _max_visited(config.max_points_visited()) {}

bool PointsLimit::_proceed(ea::algorithm::Algorithm& algorithm) {
  bool stop = algorithm.inaccurate_points() >= _max_visited;
  IPS_INFO_IF_T(
      LIMIT, stop,
      "The number of visited points reached the specified maximum");
  return !stop;
}

REGISTER_PROTO(Limit, PointsLimit, points_limit_config);

}  // namespace ea::limit