#ifndef EA_SELECTOR_H
#define EA_SELECTOR_H

#include <memory>

#include "evol/include/instance.h"
#include "evol/include/types.h"


namespace ea::selector {

class ISelector {
public:
  virtual ~ISelector() = default;

  /**
   * Performs selection of instances from population.
   */
  virtual void select(types::Population& population) = 0;
};

using RSelector = std::shared_ptr<ISelector>;

} // namespace ea::selector


#endif // EA_SELECTOR_H
