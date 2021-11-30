#ifndef EVOL_CROSSOVER_H
#define EVOL_CROSSOVER_H

#include "evol/include/domain/Instance.h"

namespace ea::method {

class Crossover {
 public:
  virtual void apply(instance::Instance& a, instance::Instance& b) = 0;
};

using RCrossover = std::shared_ptr<Crossover>;

}  // namespace ea::method

#endif  // EVOL_CROSSOVER_H
