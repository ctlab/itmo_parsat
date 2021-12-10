#ifndef EVOL_TWOPOINTCROSSOVER_H
#define EVOL_TWOPOINTCROSSOVER_H

#include "evol/include/method/cross/Crossover.h"

namespace ea::method {

class TwoPointCrossover : public Crossover {
 public:
  void apply(domain::Instance& a, domain::Instance& b) override;
};

}  // namespace ea::method

#endif  // EVOL_TWOPOINTCROSSOVER_H
