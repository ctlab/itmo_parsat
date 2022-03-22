#ifndef EVOL_INSTANCE_H
#define EVOL_INSTANCE_H

#include <set>

#include "core/sat/prop/Prop.h"
#include "core/evol/instance/SharedData.h"
#include "core/evol/method/preprocess/Preprocess.h"
#include "core/domain/assignment/RandomSearch.h"
#include "core/domain/assignment/FullSearch.h"
#include "core/domain/VarView.h"
#include "core/domain/Vars.h"
#include "util/LRUCache.h"

namespace ea::instance {
class Instance;
}  // namespace ea::instance

std::ostream& operator<<(std::ostream&, ea::instance::Instance const& instance);

namespace ea::instance {

/**
 * @brief This class represents (E|G)A instance used in this project.
 */
class Instance {
 public:
 public:
  /**
   * @param prop the propagate engine used for fitness estimation
   * @param shared_data the data shared between all instances
   * @param preprocess the preprocess data
   */
  explicit Instance(
      core::sat::prop::RProp prop, RSharedData shared_data,
      preprocess::RPreprocess preprocess);

  /**
   * @return variables which this instance represents.
   */
  core::domain::Vars& get_vars() noexcept;

  /**
   * @see Instance::get_vars()
   */
  core::domain::Vars const& get_vars() const noexcept;

  /**
   * @brief Clones the instance (used to create new instances through mutation
   * or crossover).
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

  core::LRUCache<core::bit_mask_t, Fitness>& _cache() noexcept;

  uint64_t& _inaccurate_points() noexcept;

  core::domain::VarView const& _var_view() const noexcept;

 private:
  bool _cached = false;
  preprocess::RPreprocess _preprocess;
  core::sat::prop::RProp _prop;
  RSharedData _shared;
  core::domain::Vars _vars;
  mutable Fitness fit_{};

 public:
  friend std::ostream& ::operator<<(
      std::ostream&, ea::instance::Instance const& instance);
};

bool operator<(Instance& a, Instance& b);

MAKE_REFS(Instance);

using Population = std::vector<RInstance>;

MAKE_REFS(Population);

}  // namespace ea::instance

#endif  // EVOL_INSTANCE_H
