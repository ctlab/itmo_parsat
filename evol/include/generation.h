#ifndef EA_GENERATION_H
#define EA_GENERATION_H

#include "evol/include/types.h"

namespace ea::generation {

class IGeneration {
 public:
  virtual ~IGeneration() = default;

  /**
   * Updates population by generating new instances.
   */
  virtual void generate(types::Population& population) = 0;
};

using RGeneration = std::shared_ptr<IGeneration>;

}  // namespace ea::generation

#endif  // EA_GENERATION_H
