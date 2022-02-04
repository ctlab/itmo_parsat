#include "core/evol/limit/StallLimit.h"

#include "core/evol/algorithm/Algorithm.h"

namespace ea::limit {

StallLimit::StallLimit(StallLimitConfig const& config)
    : _steps_before_stall(config.steps_before_stall()) {}

bool StallLimit::_proceed(ea::algorithm::Algorithm& algorithm) {
  auto const& fitness = algorithm.get_best().fitness();
  auto rho_value = fitness.rho;
  auto size = fitness.size;

  if (rho_value == _last_rho && size == _last_size) {
    if (++_steps_already == _steps_before_stall) {
      IPS_INFO("StallLimit: enough similar instances.");
      return false;
    } else {
      return true;
    }
  } else {
    _steps_already = 0;
    _last_size = size;
    _last_rho = rho_value;
    return true;
  }
}

REGISTER_PROTO(Limit, StallLimit, stall_limit_config);

}  // namespace ea::limit
