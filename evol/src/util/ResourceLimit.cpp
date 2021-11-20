#include "evol/include/util/ResourceLimit.h"
#include "evol/include/util/mem_usage.h"
#include "evol/include/util/Registry.h"

#include <glog/logging.h>

namespace ea::limit {

ResourceLimit::ResourceLimit(ResourceLimitConfig config)
    : memory_limit_kb_(config.memory_limit_kb())
    , time_limit_sec_(config.time_limit_sec()) {
}

bool ResourceLimit::proceed() {
  std::chrono::duration<double> elapsed = std::chrono::system_clock::now() - start_;
  bool time_ok = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() < time_limit_sec_;
  LOG_IF(INFO, !time_ok) << "Time limit exceeded.";
  bool mem_ok = getCurrentRSS() < memory_limit_kb_ * 1024;
  LOG_IF(INFO, !mem_ok) << "Memory limit exceeded.";
  return time_ok && mem_ok;
}

void ResourceLimit::start() {
  start_ = std::chrono::system_clock::now();
}

REGISTER_PROTO(Limit, ResourceLimit, resource_limit_config);

}  // namespace ea::limit
