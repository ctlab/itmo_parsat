#ifndef EA_LIMIT_H
#define EA_LIMIT_H

#include <memory>

#include "util/Registry.h"
#include "util/Reference.h"
#include "core/proto/solve_config.pb.h"

namespace ea::algorithm {

class Algorithm;

}  // namespace ea::algorithm

namespace ea::limit {

class ConjLimit;

/**
 * @brief This is an interface for all limits that control the execution of an
 * algorithm.
 */
class Limit {
  friend class ConjLimit;

 public:
  virtual ~Limit() = default;

  /**
   * @return if algorithm can and should proceed.
   * All implementations automatically check if there are unvisited points left
   * and if a strong backdoor set has been found.
   */
  bool proceed(ea::algorithm::Algorithm& algorithm);

  /**
   * @return if the algorithm should proceed.
   */
  virtual void start();

 protected:
  virtual bool _proceed(ea::algorithm::Algorithm& algorithm) = 0;
};

MAKE_REFS(Limit);

DEFINE_REGISTRY(Limit, LimitConfig, limit);

}  // namespace ea::limit

#endif  // EA_LIMIT_H
