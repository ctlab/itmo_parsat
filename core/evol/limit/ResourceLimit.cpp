#include "core/evol/limit/ResourceLimit.h"

#include "core/evol/algorithm/Algorithm.h"

namespace ea::limit {

ResourceLimit::ResourceLimit(const ResourceLimitConfig& config)
    : _memory_limit_kb(config.memory_limit_kb()), _time_limit_sec(config.time_limit_sec()) {}

bool ResourceLimit::_proceed(ea::algorithm::Algorithm&) {
  bool time_ok = true, mem_ok = true;
  if (_time_limit_sec) {
    std::chrono::duration<double> elapsed = util::clock_t::now() - _start;
    time_ok = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count() < _time_limit_sec;
    IPS_INFO_IF_T(LIMIT, !time_ok, "Time limit exceeded.");
  }
  if (_memory_limit_kb) {
    mem_ok = getCurrentRSS() < _memory_limit_kb * 1024;
    IPS_INFO_IF_T(LIMIT, !mem_ok, "Memory limit exceeded.");
  }
  return time_ok && mem_ok;
}

void ResourceLimit::start() { _start = util::clock_t::now(); }

REGISTER_PROTO(Limit, ResourceLimit, resource_limit_config);

}  // namespace ea::limit
