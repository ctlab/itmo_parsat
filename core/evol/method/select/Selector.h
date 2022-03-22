#ifndef EA_SELECTOR_H
#define EA_SELECTOR_H

#include <memory>

#include "util/Registry.h"
#include "core/evol/instance/Instance.h"
#include "core/proto/solve_config.pb.h"

namespace ea::method {

/**
 * @brief Performs instance selection.
 */
class Selector {
 public:
  virtual ~Selector() = default;

  /**
   * @brief Performs selection of `size` instances from population.
   */
  virtual void select(instance::Population& population, uint32_t size) = 0;
};

MAKE_REFS(Selector);

DEFINE_REGISTRY(Selector, SelectorConfig, selector);

}  // namespace ea::method

#endif  // EA_SELECTOR_H
