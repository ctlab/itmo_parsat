#ifndef EVOL_RESOURCELIMIT_H
#define EVOL_RESOURCELIMIT_H

#include "core/evol/limit/Limit.h"

namespace ea::limit {

class ResourceLimit : public Limit {
 public:
  /**
   * This limit controls the execution by limiting time and memory consumption of an algorithm.
   */
  ResourceLimit(ResourceLimitConfig const& config);

  /**
   * @return true iff time and memory limits are both not reached
   */
  bool proceed(ea::algorithm::Algorithm& algorithm) override;

  void start() override;

 private:
  uint32_t _memory_limit_kb;
  uint32_t _time_limit_sec;
  std::chrono::time_point<std::chrono::system_clock> _start;
};

}  // namespace ea::limit

#endif  // EVOL_RESOURCELIMIT_H
