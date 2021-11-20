#ifndef EVOL_RESOURCELIMIT_H
#define EVOL_RESOURCELIMIT_H

#include "evol/include/util/Limit.h"
#include "evol/proto/config.pb.h"

namespace ea::limit {

class ResourceLimit : public Limit {
 public:
  ResourceLimit(ResourceLimitConfig config);

  bool proceed() override;

  void start() override;

 private:
    uint32_t memory_limit_kb_;
    uint32_t time_limit_sec_;

    std::chrono::time_point<std::chrono::system_clock> start_;
};

}  // namespace ea::limit

#endif  // EVOL_RESOURCELIMIT_H
