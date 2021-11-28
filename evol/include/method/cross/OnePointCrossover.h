#ifndef EVOL_ONEPOINTCROSSOVER_H
#define EVOL_ONEPOINTCROSSOVER_H

#include "evol/include/method/cross/Crossover.h"

namespace ea::method {

class OnePointCrossover : public Crossover {
 public:
  OnePointCrossover() = default;

  void apply(instance::RInstance& a, instance::RInstance& b) override;
};

}  // namespace ea::method

#endif  // EVOL_ONEPOINTCROSSOVER_H
