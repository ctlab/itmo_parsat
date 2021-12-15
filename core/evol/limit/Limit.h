#ifndef EA_LIMIT_H
#define EA_LIMIT_H

#include <memory>

#include "core/util/Registry.h"
#include "core/proto/solve_config.pb.h"
#include "core/evol/instance/Instance.h"

namespace ea::limit {

class Limit {
 public:
  virtual ~Limit() = default;

  /**
   * Returns true if algorithm can and should proceed.
   */
  virtual bool proceed(instance::Population const& population) = 0;

  /**
   * Indicates that limit must start tracking parameters
   * that it should track (for ex. memory/time consumption).
   */
  virtual void start() {}
};

using RLimit = std::shared_ptr<Limit>;

DEFINE_REGISTRY(Limit, LimitConfig, limit);

}  // namespace ea::limit

#endif  // EA_LIMIT_H
