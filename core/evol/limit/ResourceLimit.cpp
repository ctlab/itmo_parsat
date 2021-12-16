#include "core/evol/limit/ResourceLimit.h"

#include "core/util/mem_usage.h"

namespace ea::limit {

ResourceLimit::ResourceLimit(const ResourceLimitConfig& config)
    : memory_limit_kb_(config.memory_limit_kb()), time_limit_sec_(config.time_limit_sec()) {}

bool ResourceLimit::proceed(instance::Population const&) {
  bool time_ok = true, mem_ok = true;
  if (time_limit_sec_) {
    std::chrono::duration<double> elapsed = std::chrono::system_clock::now() - start_;
    time_ok = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() < time_limit_sec_;
    IPS_INFO_IF(!time_ok, "Time limit exceeded.");
  }
  if (memory_limit_kb_) {
    mem_ok = getCurrentRSS() < memory_limit_kb_ * 1024;
    IPS_INFO_IF(!mem_ok, "Memory limit exceeded.");
  }
  return time_ok && mem_ok;
}

void ResourceLimit::start() {
  start_ = std::chrono::system_clock::now();
}

REGISTER_PROTO(Limit, ResourceLimit, LimitConfig, resource_limit_config);

}  // namespace ea::limit
