#ifndef EVOL_TWOPOINTCROSSOVER_H
#define EVOL_TWOPOINTCROSSOVER_H

#include "core/evol/method/cross/Crossover.h"

namespace ea::method {

class TwoPointCrossover : public Crossover {
 public:
  void apply(instance::Instance& a, instance::Instance& b) override;
};

}  // namespace ea::method

#endif  // EVOL_TWOPOINTCROSSOVER_H
