#ifndef EVOL_INSTANCE_H
#define EVOL_INSTANCE_H

#include <set>

#include "evol/include/sat/Solver.h"
#include "evol/include/config/Configuration.h"
#include "evol/include/domain/Assignment.h"
#include "evol/include/domain/Fitness.h"
#include "evol/include/domain/VarView.h"
#include "evol/include/domain/Vars.h"
#include "evol/include/util/stream.h"
#include "evol/include/util/Cache.h"
#include "evol/proto/config.pb.h"

namespace ea::domain {

class Instance;

}  // namespace ea::domain

std::ostream& operator<<(std::ostream&, ea::domain::Instance& instance);

namespace ea::domain {

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
    Cache<std::vector<bool>, Fitness> cache{};
    VarView var_view{};
    uint32_t inaccurate_points = 0;
    uint32_t omega_x{};
  };

  using RSharedData = std::shared_ptr<SharedData>;

 public:
  explicit Instance(sat::RSolver solver, RSharedData shared_data);

  Vars& get_vars() noexcept;

  [[nodiscard]] Instance* clone();

  Fitness const& fitness();

  [[nodiscard]] bool is_cached() const noexcept;

  [[nodiscard]] VarView const& var_view() const noexcept;

  [[nodiscard]] size_t num_vars() const noexcept;

 private:
  void _calc_fitness();

  SamplingConfig& _sampling_config() noexcept;

  Cache<std::vector<bool>, Fitness>& _cache() noexcept;

  uint32_t& _inaccurate_points() noexcept;

  VarView& _var_view() noexcept;

 private:
  bool cached_ = false;
  std::shared_ptr<sat::Solver> solver_;
  std::shared_ptr<SharedData> shared_;
  Vars vars_;
  Fitness fit_{};

 private:
  friend std::ostream& ::operator<<(std::ostream&, ea::domain::Instance& instance);
};

bool operator<(Instance& a, Instance& b);

}  // namespace ea::domain

#endif  // EVOL_INSTANCE_H
