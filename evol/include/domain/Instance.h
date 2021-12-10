#ifndef EVOL_INSTANCE_H
#define EVOL_INSTANCE_H

#include <set>

#include "evol/include/sat/Solver.h"
#include "evol/include/config/Configuration.h"
#include "evol/include/domain/Assignment.h"
#include "evol/include/domain/Fitness.h"
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

void get_fit(Instance&);

class Instance {
 private:
  struct SamplingConfig {
    uint32_t samples;
    uint32_t can_scale;
    double scale;

   public:
    SamplingConfig(uint32_t samples, uint32_t can_scale, double scale);

    void do_scale();
  };

 public:
  explicit Instance(InstanceConfig const& config, sat::RSolver solver);

  [[nodiscard]] Vars const& get_vars() const noexcept;

  Vars& get_vars() noexcept;

  [[nodiscard]] Instance* clone();

  Fitness const& fitness();

  [[nodiscard]] bool is_cached() const noexcept;

 private:
  void _init_heuristic(InstanceConfig const& config);

  void _calc_fitness();

 private:
  using cache_t = Cache<std::vector<bool>, Fitness>;
  bool cached_ = false;
  std::shared_ptr<sat::Solver> solver_;
  std::shared_ptr<SamplingConfig> sampling_config_;
  std::shared_ptr<cache_t> instance_cache_;

  uint32_t omega_x;
  Vars vars_;
  Fitness fit_{};

 private:
  static std::map<int, int> var_map_;
  static std::atomic_uint32_t inaccurate_points_;

 public:
  static size_t num_vars() noexcept;

  static uint32_t inaccurate_points();

  static std::map<int, int> const& var_map() noexcept;

 private:
  friend std::ostream& ::operator<<(std::ostream&, ea::domain::Instance& instance);
};

RInstance createInstance(sat::RSolver const& solver);

bool operator<(Instance& a, Instance& b);

}  // namespace ea::domain


#endif  // EVOL_INSTANCE_H
