#ifndef EVOL_CROSSOVER_H
#define EVOL_CROSSOVER_H

#include "core/util/Registry.h"
#include "core/evol/instance/Instance.h"
#include "core/proto/solve_config.pb.h"

namespace ea::method {

/**
 * @brief Crossover method interface.
 */
class Crossover {
 public:
  virtual ~Crossover() = default;

  /**
   * @brief applies crossover to two instances.
   * @param a the first instance.
   * @param b the second instance.
   */
  virtual void apply(instance::Instance& a, instance::Instance& b) = 0;
};

using RCrossover = std::shared_ptr<Crossover>;

DEFINE_REGISTRY(Crossover, CrossoverConfig, crossover);

}  // namespace ea::method

#endif  // EVOL_CROSSOVER_H
