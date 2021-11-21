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

 private:
  void assure_initialized();

 public:
  explicit RBDInstance(RBDInstanceConfig const& config);

  void set_solver(sat::RSolver const& solver) override;

  void get_assumptions(Minisat::vec<Minisat::Lit>& assumptions) override;

  [[nodiscard]] Instance* clone() override;

  [[nodiscard]] double fitness() override;

  [[nodiscard]] RInstance crossover(RInstance const& other) override;

  void mutate() override;

 private:
  RBDInstanceConfig config_{};

  /* TODO: remove all consts and mutables. */
  bool deferred_init_heuristic_ = false;
  bool deferred_init_random_ = false;
  double cached_ = 0.;
  uint64_t max_sampling_size_ = 0.;
  CacheState cache_state_ = NO_CACHE;

  std::set<unsigned> vars_;
  std::shared_ptr<sat::Solver> solver_;
};

}  // namespace ea::instance

#endif  // EVOL_RBDINSTANCE_H
