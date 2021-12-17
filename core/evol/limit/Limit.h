#ifndef EA_LIMIT_H
#define EA_LIMIT_H

#include <memory>

#include "core/util/Registry.h"
#include "core/proto/solve_config.pb.h"

namespace ea::algorithm {

class Algorithm;

}  // namespace ea::algorithm

namespace ea::limit {

/**
 * This is an interface for all limits that control the execution of an algorithm.
 */
class Limit {
 public:
  virtual ~Limit() = default;

  /**
   * @return if algorithm can and should proceed.
   */
  virtual bool proceed(ea::algorithm::Algorithm& algorithm) = 0;

  /**
   * @return true iff the algorithm should proceed.
   */
  virtual void start() {}
};

using RLimit = std::shared_ptr<Limit>;

DEFINE_REGISTRY(Limit, LimitConfig, limit);

}  // namespace ea::limit

#endif  // EA_LIMIT_H
