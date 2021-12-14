#ifndef EA_SELECTOR_H
#define EA_SELECTOR_H

#include <memory>

#include "core/util/Registry.h"
#include "evol/instance/Instance.h"
#include "core/proto/config.pb.h"

namespace ea::method {

class Selector {
 public:
  virtual ~Selector() = default;

  /**
   * Performs selection of `size` instances from population.
   */
  virtual void select(instance::Population& population, size_t size) = 0;
};

using RSelector = std::shared_ptr<Selector>;

DEFINE_REGISTRY(Selector, SelectorConfig, selector);

}  // namespace ea::method

#endif  // EA_SELECTOR_H
