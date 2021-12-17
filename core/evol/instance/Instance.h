#ifndef EVOL_INSTANCE_H
#define EVOL_INSTANCE_H

#include <set>

#include "core/evol/instance/SharedData.h"
#include "core/sat/Solver.h"
#include "core/domain/Assignment.h"
#include "core/domain/VarView.h"
#include "core/domain/Vars.h"
#include "core/util/stream.h"
#include "core/util/Logger.h"
#include "core/util/LRUCache.h"
#include "core/util/Generator.h"

namespace ea::instance {

class Instance;

}  // namespace ea::instance

std::ostream& operator<<(std::ostream&, ea::instance::Instance const& instance);

namespace ea::instance {

class Instance;

using RInstance = std::shared_ptr<Instance>;

using Population = std::vector<RInstance>;

/**
 * This class represents (E|G)A instance used in this project.
 */
class Instance {
 public:
 public:
  /**
   * @param solver solver that will be used to propagate assignments.
   * @param shared_data shared data (created by algorithm)
   */
  explicit Instance(core::sat::RSolver solver, RSharedData shared_data);

  /**
   * @return variables which this instance represents.
   */
  core::domain::Vars& get_vars() noexcept;
  core::domain::Vars const& get_vars() const noexcept;

  /**
   * Clones the instance (used to create new instances through mutation or crossover).
   */
  [[nodiscard]] Instance* clone();

  /**
   * Calculates instance's fitness if it is not cached.
   * @return the fitness value
   */
  Fitness const& fitness();

  /**
   * Returns the fitness value if it is cached.
   * Otherwise, terminate is called.
   */
  Fitness const& fitness() const noexcept;

  /**
   * Checks whether this instance is cached (locally or in shared cache).
   */
  [[nodiscard]] bool is_cached() const noexcept;

  /**
   * @return the number of variables in this instance
   */
  [[nodiscard]] size_t num_vars() const noexcept;

 private:
  void _calc_fitness();

  SharedData::SamplingConfig& _sampling_config() noexcept;

  core::LRUCache<std::vector<bool>, Fitness>& _cache() noexcept;

  uint32_t& _inaccurate_points() noexcept;

  core::domain::VarView& _var_view() noexcept;

 private:
  bool cached_ = false;
  std::shared_ptr<core::sat::Solver> solver_;
  std::shared_ptr<SharedData> shared_;
  core::domain::Vars vars_;
  Fitness fit_{};

 private:
  friend std::ostream& ::operator<<(std::ostream&, ea::instance::Instance const& instance);
};

bool operator<(Instance& a, Instance& b);

}  // namespace ea::instance

#endif  // EVOL_INSTANCE_H
