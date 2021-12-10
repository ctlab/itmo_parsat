#ifndef EA_SELECTOR_H
#define EA_SELECTOR_H

#include <memory>

#include "evol/include/domain/Instance.h"

namespace ea::method {

class Selector {
 public:
  virtual ~Selector() = default;

  /**
   * Performs selection of `size` instances from population.
   */
  virtual void select(domain::Population& population, size_t size) = 0;
};

using RSelector = std::shared_ptr<Selector>;

}  // namespace ea::method

#endif  // EA_SELECTOR_H
