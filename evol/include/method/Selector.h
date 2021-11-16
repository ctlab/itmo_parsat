#ifndef EA_SELECTOR_H
#define EA_SELECTOR_H

#include <memory>

#include "evol/include/domain/Instance.h"

namespace ea::selector {

class Selector {
 public:
  virtual ~Selector() = default;

  /**
   * Performs selection of instances from population.
   */
  virtual void select(instance::Population& population) = 0;
};

using RSelector = std::shared_ptr<Selector>;

}  // namespace ea::selector

#endif  // EA_SELECTOR_H
