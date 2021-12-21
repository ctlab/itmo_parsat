#include "core/evol/limit/Limit.h"
#include "core/evol/algorithm/Algorithm.h"

namespace ea::limit {

bool Limit::proceed(ea::algorithm::Algorithm& algorithm) {
  return _proceed(algorithm) && algorithm.has_unvisited_points();
}

}  // namespace ea::limit