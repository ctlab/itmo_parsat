#include "evol/include/limit/ConjLimit.h"

#include "evol/include/util/Registry.h"

namespace ea::limit {

ConjLimit::ConjLimit(const ConjLimitConfig& config) {
  for (int i = 0; i < config.limit_type_size(); ++i) {
    limits_.push_back(registry::Registry::resolve_limit(config.limit_type(i)));
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

REGISTER_PROTO(Limit, ConjLimit, conj_limit_config);

}  // namespace ea::limit