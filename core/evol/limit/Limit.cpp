#include "core/evol/limit/Limit.h"
#include "core/evol/algorithm/Algorithm.h"

namespace ea::limit {

bool Limit::proceed(ea::algorithm::Algorithm& algorithm) {
  return algorithm.has_unvisited_points() && _proceed(algorithm) &&
         !algorithm.get_best().is_sbs();
}

void Limit::start() {}

}  // namespace ea::limit