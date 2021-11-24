#ifndef EA_INSTANCE_H
#define EA_INSTANCE_H

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "evol/include/sat/Solver.h"

namespace ea::instance {

struct Fitness {
  double rho;
  int32_t pow_r;
  int32_t pow_nr;

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
   * Returns variables bit vector.
   */
  virtual std::vector<bool> get_variables() = 0;

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

 public:
  static std::map<int, int> var_map;
};

bool operator<(Instance& a, Instance& b);

class Assignment {
 public:
  Assignment(std::map<int, int>& var_map, std::vector<bool> const& vars);

  Minisat::vec<Minisat::Lit> const& operator()() const;

  virtual bool operator++() = 0;

 protected:
  Minisat::vec<Minisat::Lit> assignment_;
};

class FullSearch : public Assignment {
 public:
  FullSearch(std::map<int, int>& var_map, std::vector<bool> const& vars);

  bool operator++() override;
};

class RandomAssignments : public Assignment {
 public:
  RandomAssignments(std::map<int, int>& var_map, std::vector<bool> const& vars);

  bool operator++() override;
};

}  // namespace ea::instance

#endif  // EA_INSTANCE_H
