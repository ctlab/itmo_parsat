#ifndef EVOL_INSTANCE_H
#define EVOL_INSTANCE_H

#include <set>

#include "evol/include/sat/Solver.h"
#include "evol/include/config/Configuration.h"
#include "evol/include/domain/Assignment.h"
#include "evol/include/util/stream.h"
#include "evol/proto/config.pb.h"

namespace ea::instance {

class Instance;

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

void get_fit(Instance&);

class Instance {
 private:
  enum CacheState {
    CACHE,
    NO_CACHE,
  };

  struct Vars {
    std::vector<bool> bit_mask;

   public:
    void flip(size_t pos);
  };

 public:
  explicit Instance(InstanceConfig const& config, sat::RSolver solver);

  [[nodiscard]] std::vector<bool> const& get_mask() const noexcept;

  std::vector<bool>& get_mask() noexcept;

  [[nodiscard]] std::vector<int> get_variables() const noexcept;

  void flip_var(size_t var);

  [[nodiscard]] Instance* clone();

  Fitness const& fitness();

 private:
  void _init_heuristic(InstanceConfig const& config);

  void _calc_fitness();

 private:
  std::shared_ptr<sat::Solver> solver_;
  CacheState cache_state_ = NO_CACHE;
  Fitness fit_{};
  uint32_t max_sampling_size_ = 0, omega_x;
  Vars vars_;

 private:
  static std::map<int, int> var_map_;
  static std::atomic_uint32_t inaccurate_points_;

 public:
  static size_t num_vars() noexcept;

  static uint32_t inaccurate_points();

  static std::map<int, int> const& var_map() noexcept;

  static bool is_cached(std::vector<bool> const& vars) noexcept;
};

RInstance createInstance(sat::RSolver const& solver);

bool operator<(Instance& a, Instance& b);

}  // namespace ea::instance

std::ostream& operator<<(std::ostream&, ea::instance::Instance& instance);

#endif  // EVOL_INSTANCE_H
