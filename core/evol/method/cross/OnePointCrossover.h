#ifndef EVOL_ONEPOINTCROSSOVER_H
#define EVOL_ONEPOINTCROSSOVER_H

#include "core/evol/method/cross/Crossover.h"

namespace ea::method {

/**
 * @brief Standard one-point crossover.
 */
class OnePointCrossover : public Crossover {
 public:
  OnePointCrossover() = default;

  void apply(instance::Instance& a, instance::Instance& b) override;
};

}  // namespace ea::method

#endif  // EVOL_ONEPOINTCROSSOVER_H
