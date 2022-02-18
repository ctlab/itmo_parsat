#ifndef EVOL_INSTANCE_H
#define EVOL_INSTANCE_H

#include <set>

#include "core/sat/prop/Prop.h"
#include "core/evol/instance/SharedData.h"
#include "core/domain/assignment/RandomSearch.h"
#include "core/domain/assignment/FullSearch.h"
#include "core/domain/VarView.h"
#include "core/domain/Vars.h"
#include "util/stream.h"
#include "util/Logger.h"
#include "util/LRUCache.h"
#include "util/Generator.h"

namespace ea::instance {

class Instance;

}  // namespace ea::instance

std::ostream& operator<<(std::ostream&, ea::instance::Instance const& instance);

namespace ea::instance {

class Instance;

using RInstance = std::shared_ptr<Instance>;

using Population = std::vector<RInstance>;

/**
 * @brief This class represents (E|G)A instance used in this project.
 */
class Instance {
 public:
 public:
  /**
   * @param solver solver that will be used to propagate assignments.
   * @param shared_data shared data (created by algorithm)
   */
  explicit Instance(core::sat::prop::RProp prop, RSharedData shared_data);

  /**
   * @return variables which this instance represents.
   */
  core::domain::Vars& get_vars() noexcept;

  /**
   * @see Instance::get_vars()
   */
  core::domain::Vars const& get_vars() const noexcept;

  /**
   * @brief Clones the instance (used to create new instances through mutation or crossover).
   */
  [[nodiscard]] Instance* clone();

  /**
   * @brief Calculates instance's fitness if it is not cached.
   * @return the fitness value
   */
  Fitness const& fitness();

  /**
   * @brief Returns the fitness value if it is cached.
   * Otherwise, terminate is called.
   */
  Fitness const& fitness() const noexcept;

  /**
   * @brief Checks whether this instance is cached (locally or in shared cache).
   */
  [[nodiscard]] bool is_cached() const noexcept;

  /**
   * @todo The fact that this method is here is very bad.
   * @return the number of variables in the formula (not in this instance)
   */
  [[nodiscard]] uint32_t num_vars() const noexcept;

  /**
   * @return if this instance is a strong backdoor set
   */
  [[nodiscard]] bool is_sbs() const noexcept;

  /**
   * @return the number of variables in this variables set
   */
  [[nodiscard]] uint32_t size() const noexcept;

 private:
  void _calc_fitness();

  void _calc_fitness(uint32_t samples, uint32_t steps_left);

  SharedData::SamplingConfig& _sampling_config() noexcept;

  core::LRUCache<std::vector<bool>, Fitness>& _cache() noexcept;

  uint64_t& _inaccurate_points() noexcept;

  core::domain::VarView& _var_view() noexcept;

 private:
  bool _cached = false;
  core::sat::prop::RProp _prop;
  std::shared_ptr<SharedData> _shared;
  core::domain::Vars _vars;

  // Mutability here is actually a design flaw, which will probably be fixed in the future.
  // It is needed to update fitness from cache when `fitness() const` is called.
  mutable Fitness fit_{};

 public:
  friend std::ostream& ::operator<<(std::ostream&, ea::instance::Instance const& instance);
};

bool operator<(Instance& a, Instance& b);

}  // namespace ea::instance

#endif  // EVOL_INSTANCE_H
