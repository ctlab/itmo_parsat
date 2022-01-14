#ifndef EA_LIMIT_H
#define EA_LIMIT_H

#include <memory>

#include "core/util/Registry.h"
#include "core/proto/solve_config.pb.h"

namespace ea::algorithm {

class Algorithm;

}  // namespace ea::algorithm

namespace ea::limit {

class ConjLimit;

/**
 * @brief This is an interface for all limits that control the execution of an algorithm.
 */
class Limit {
  friend class ConjLimit;

 public:
  virtual ~Limit() = default;

  /**
   * @return if algorithm can and should proceed.
   * All implementations automatically check if there are unvisited points left.
   */
  bool proceed(ea::algorithm::Algorithm& algorithm);

  /**
   * @return true iff the algorithm should proceed.
   */
  virtual void start() {}

 protected:
  virtual bool _proceed(ea::algorithm::Algorithm& algorithm) = 0;
};

using RLimit = std::shared_ptr<Limit>;

DEFINE_REGISTRY(Limit, LimitConfig, limit);

}  // namespace ea::limit

#endif  // EA_LIMIT_H
