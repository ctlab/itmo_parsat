#ifndef EA_INSTANCE_H
#define EA_INSTANCE_H

#include <memory>
#include <vector>

#include "evol/include/sat/Solver.h"

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
  [[nodiscard]] virtual Instance* clone() = 0;

  /**
   * Sets solver to the specified one.
   */
  virtual void set_solver(sat::RSolver const& solver) {};

  /**
   * Returns random assumptions for this rho-backdoor.
   */
  virtual void get_assumptions(Minisat::vec<Minisat::Lit>& assumptions) = 0;

  /**
   * Calculates instance's fitness function
   * Used to perform selection.
   */
  virtual double fitness() = 0;

  /**
   * Estimates rho-backdoor's rho value.
   */
  virtual double rho() = 0;

  /**
   * Performs crossover operation with other instance.
   */
  [[nodiscard]] virtual RInstance crossover(RInstance const& a) = 0;

  /**
   * Performs mutation operation.
   */
  virtual void mutate() = 0;
};

bool operator<(Instance& a, Instance& b);

}  // namespace ea::instance

#endif  // EA_INSTANCE_H
