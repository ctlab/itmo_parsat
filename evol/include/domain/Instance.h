#ifndef EA_INSTANCE_H
#define EA_INSTANCE_H

#include <memory>
#include <vector>

namespace ea::instance {

class Instance;

using RInstance = std::shared_ptr<Instance>;

using Population = std::vector<RInstance>;

using RPopulation = std::shared_ptr<Population>;

class Instance {
 public:
  virtual ~Instance() = default;

  /**
   * Clones the instance.
   */
  [[nodiscard]] virtual Instance* clone() const = 0;

  /**
   * Calculates instance's fitness function
   * Used to perform selection.
   */
  [[nodiscard]] virtual double fitness() const = 0;

  /**
   * Performs crossover operation with other instance.
   */
  [[nodiscard]] virtual RInstance crossover(RInstance const& a) const = 0;

  /**
   * Performs mutation operation.
   */
  virtual void mutate() = 0;
};

bool operator<(Instance const& a, Instance const& b);

}  // namespace ea::instance

#endif  // EA_INSTANCE_H
