#include "evol/limit/ConjLimit.h"

namespace ea::limit {

ConjLimit::ConjLimit(const ConjLimitConfig& config) {
  for (int i = 0; i < config.limit_config_size(); ++i) {
    limits_.emplace_back(LimitRegistry::resolve(config.limit_config(i)));
  }
}

bool ConjLimit::proceed(instance::Population const& population) {
  return std::all_of(limits_.begin(), limits_.end(), [&population](RLimit& limit) {
    return limit->proceed(population);
  });
}

void ConjLimit::start() {
  std::for_each(limits_.begin(), limits_.end(), [](RLimit& limit) { limit->start(); });
}

REGISTER_PROTO(Limit, ConjLimit, LimitConfig, conj_limit_config);

}  // namespace ea::limit