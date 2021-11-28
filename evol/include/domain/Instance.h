#ifndef EVOL_INSTANCE_H
#define EVOL_INSTANCE_H

#include <set>

#include "evol/include/sat/Solver.h"
#include "evol/include/config/Configuration.h"
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

using RPopulation = std::shared_ptr<Population>;

class Instance {
 private:
  enum CacheState {
    CACHE,
    NO_CACHE,
  };

  struct Vars {
    std::vector<bool> bit_mask;
    size_t size = 0;

   public:
    void flip(size_t pos);
  };

 public:
  explicit Instance(InstanceConfig const& config, sat::RSolver solver);

  [[nodiscard]] std::vector<bool> const& get_variables() const noexcept;

  std::vector<bool>& get_variables() noexcept;

  void flip_var(size_t var);

  void recalc_vars() noexcept;

  [[nodiscard]] Instance* clone();

  [[nodiscard]] size_t size() const noexcept;

  Fitness const& fitness();

 private:
  void _init_heuristic(InstanceConfig const& config);

 private:
  std::shared_ptr<sat::Solver> solver_;
  CacheState cache_state_ = NO_CACHE;
  Fitness fit_{};
  uint32_t max_sampling_size_ = 0, omega_x;
  Vars vars_;

 private:
  static std::map<int, int> var_map_;

 public:
  static size_t num_vars() noexcept;

  static std::map<int, int> const& var_map() noexcept;
};

RInstance createInstance(sat::RSolver const& solver);

bool operator<(Instance& a, Instance& b);

class Assignment {
 public:
  explicit Assignment(std::vector<bool> const& vars);

  Minisat::vec<Minisat::Lit> const& operator()() const;

  virtual bool operator++() = 0;

 protected:
  Minisat::vec<Minisat::Lit> assignment_;
};

class FullSearch : public Assignment {
 public:
  explicit FullSearch(std::vector<bool> const& vars);

  bool operator++() override;
};

class RandomAssignments : public Assignment {
 public:
  explicit RandomAssignments(std::vector<bool> const& vars);

  bool operator++() override;
};

}  // namespace ea::instance

#endif  // EVOL_INSTANCE_H
