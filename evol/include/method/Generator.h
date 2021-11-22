#ifndef EA_GENERATION_H
#define EA_GENERATION_H

#include "evol/include/domain/Instance.h"

namespace ea::generator {

class Generator {
 public:
  virtual ~Generator() = default;

  /**
   * Updates population by generating new instances.
   */
  virtual void generate(instance::Population& population) = 0;
};

using RGeneration = std::shared_ptr<Generator>;

}  // namespace ea::generator

#endif  // EA_GENERATION_H
