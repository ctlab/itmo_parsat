#ifndef EVOL_ONEPOINTCROSSOVER_H
#define EVOL_ONEPOINTCROSSOVER_H

#include "evol/include/method/cross/Crossover.h"

namespace ea::method {

class OnePointCrossover : public Crossover {
 public:
  OnePointCrossover() = default;

  void apply(domain::Instance& a, domain::Instance& b) override;
};

}  // namespace ea::method

#endif  // EVOL_ONEPOINTCROSSOVER_H
