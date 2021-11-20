#ifndef EA_LIMIT_H
#define EA_LIMIT_H

#include <memory>

#include "evol/include/domain/Instance.h"

namespace ea::limit {

class Limit {
 public:
  virtual ~Limit() = default;

  /**
   * Returns true if algorithm can and should proceed.
   */
  virtual bool proceed(instance::RPopulation population) = 0;

  /**
   * Indicates that limit must start tracking parameters
   * that it should track (for ex. memory/time consumption).
   */
  virtual void start() {}
};

using RLimit = std::shared_ptr<Limit>;

}  // namespace ea::limit

#endif  // EA_LIMIT_H
