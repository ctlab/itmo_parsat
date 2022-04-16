#include "core/evol/limit/ConjLimit.h"

namespace ea::limit {

ConjLimit::ConjLimit(const ConjLimitConfig& config) {
  for (int i = 0; i < config.limit_config_size(); ++i) {
    _limits.emplace_back(LimitRegistry::resolve(config.limit_config(i)));
  }
}

bool ConjLimit::_proceed(ea::algorithm::Algorithm& algorithm) {
  return std::all_of(_limits.begin(), _limits.end(), [&algorithm](RLimit& limit) {
    return limit->_proceed(algorithm);
  });
}

void ConjLimit::start() {
  std::for_each(_limits.begin(), _limits.end(), [](RLimit& limit) { limit->start(); });
}

REGISTER_PROTO(Limit, ConjLimit, conj_limit_config);

}  // namespace ea::limit