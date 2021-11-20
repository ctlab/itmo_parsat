#include "evol/include/util/ConjLimit.h"
#include "evol/include/util/Registry.h"

namespace ea::limit {

ConjLimit::ConjLimit(ConjLimitConfig config) {
  for (int i = 0; i < config.limit_type_size(); ++i) {
    limits_.push_back(registry::Registry::instance().resolve_limit(config.limit_type(i)));
  }
}

bool ConjLimit::proceed() {
  return std::all_of(limits_.begin(), limits_.end(), [] (RLimit& limit) {
    return limit->proceed();
  });
}

void ConjLimit::start() {
  std::for_each(limits_.begin(), limits_.end(), [] (RLimit& limit) {
    limit->start();
  });
}

}  // namespace ea::limit