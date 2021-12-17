#ifndef EVOL_INSTANCE_H
#define EVOL_INSTANCE_H

#include <set>

#include "core/sat/Solver.h"
#include "core/domain/Assignment.h"
#include "core/domain/VarView.h"
#include "core/domain/Vars.h"
#include "core/evol/instance/Fitness.h"
#include "core/util/stream.h"
#include "core/util/Logger.h"
#include "core/util/Cache.h"

namespace ea::instance {

class Instance;

}  // namespace ea::instance

std::ostream& operator<<(std::ostream&, ea::instance::Instance const& instance);

namespace ea::instance {

class Instance;

using RInstance = std::shared_ptr<Instance>;

using Population = std::vector<RInstance>;

class Instance {
 public:
  struct SamplingConfig {
    uint32_t samples;
    uint32_t can_scale;
    double scale;

   public:
    void do_scale();
  };

  struct SharedData {
    SamplingConfig sampling_config{};
    core::Cache<std::vector<bool>, Fitness> cache{};
    core::domain::VarView var_view{};
    uint32_t inaccurate_points = 0;
    uint32_t omega_x{};
  };

  using RSharedData = std::shared_ptr<SharedData>;

 public:
  explicit Instance(core::sat::RSolver solver, RSharedData shared_data);

  core::domain::Vars& get_vars() noexcept;

  core::domain::Vars const& get_vars() const noexcept;

  [[nodiscard]] Instance* clone();

  Fitness const& fitness();

  Fitness const& fitness() const noexcept;

  [[nodiscard]] bool is_cached() const noexcept;

  [[nodiscard]] core::domain::VarView const& var_view() const noexcept;

  [[nodiscard]] size_t num_vars() const noexcept;

 private:
  void _calc_fitness();

  SamplingConfig& _sampling_config() noexcept;

  core::Cache<std::vector<bool>, Fitness>& _cache() noexcept;

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
