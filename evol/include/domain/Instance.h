#ifndef EA_INSTANCE_H
#define EA_INSTANCE_H

#include <memory>
#include <set>
#include <vector>

#include "evol/include/sat/Solver.h"

namespace ea::instance {

struct Fitness {
  double rho;
  uint32_t pow_r;
  uint32_t pow_nr;

  /* Check whether value can be calculated using uint64 and double types. */
  [[nodiscard]] bool can_calc() const;

  /* Better check if `can_calc` before using. */
  explicit operator double() const;
};

bool operator<(Fitness const& a, Fitness const& b);

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
  virtual void set_solver(sat::RSolver const& solver){};

  /**
   * Returns variables set.
   */
  virtual std::set<unsigned> get_variables() = 0;

  /**
   * Calculates instance's fitness function
   * Used to perform selection.
   */
  virtual Fitness const& fitness() = 0;

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

class Assignment {
 public:
  explicit Assignment(std::set<unsigned> const& variables);

  Minisat::vec<Minisat::Lit> const& operator()() const;

  bool operator++();

 private:
  Minisat::vec<Minisat::Lit> asgn_;
};

}  // namespace ea::instance

#endif  // EA_INSTANCE_H
