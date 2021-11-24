#ifndef EVOL_RBDINSTANCE_H
#define EVOL_RBDINSTANCE_H

#include <set>

#include "evol/include/domain/Instance.h"
#include "evol/include/sat/Solver.h"
#include "evol/proto/config.pb.h"

namespace ea::instance {

class RBDInstance : public Instance {
 private:
  enum CacheState {
    CACHE,
    NO_CACHE,
  };

 public:
  explicit RBDInstance(RBDInstanceConfig const& config, sat::RSolver solver);

  std::vector<bool> get_variables() override;

  [[nodiscard]] Instance* clone() override;

  Fitness const& fitness() override;

  [[nodiscard]] RInstance crossover(RInstance const& other) override;

  void mutate() override;

 private:
  void _init_heuristic(RBDInstanceConfig const& config);

  void _flip_var(size_t var);

 private:
  uint32_t max_sampling_size_ = 0;
  uint32_t omega_x;
  CacheState cache_state_ = NO_CACHE;

  Fitness fit_{};
  std::vector<bool> vars_;
  uint32_t vars_size_ = 0;
  std::shared_ptr<sat::Solver> solver_;
};

std::shared_ptr<RBDInstance> createRBDInstance(sat::RSolver const& solver);

}  // namespace ea::instance

#endif  // EVOL_RBDINSTANCE_H
