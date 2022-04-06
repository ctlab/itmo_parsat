#ifndef EVOL_PARPROP_H
#define EVOL_PARPROP_H

#include <vector>
#include <atomic>
#include <variant>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <queue>

#include "util/Random.h"
#include "core/sat/prop/Prop.h"
#include "util/WorkerPool.h"

namespace core::sat::prop {

/**
 * @brief Parallel propagation implementation.
 * @note Intended to be used in parallel environment.
 */
class ParProp : public Prop {
 public:
  explicit ParProp(ParPropConfig const& config);

  void load_problem(Problem const& problem) override;

  [[nodiscard]] uint32_t num_vars() const noexcept override;

  bool propagate(lit_vec_t const& assumptions, lit_vec_t& propagated) override;

  bool propagate(lit_vec_t const& assumptions) override;

  uint64_t prop_search(
      search::USearch search_p, prop_callback_t const& callback) override;

  uint64_t prop_search(
      lit_vec_t const& base_assumption, search::USearch search_p,
      prop_callback_t const& callback) override;

  uint64_t prop_tree(lit_vec_t const& vars, uint32_t head_size) override;

 private:
  uint64_t _sum() noexcept;

 private:
  using PropWorkerPool = util::WorkerPool<RProp, uint64_t>;

 private:
  std::vector<std::future<uint64_t>> _futures;
  std::mutex _solver_m;
  PropWorkerPool _prop_worker_pool;
  RProp _seq_worker;
  uint32_t _tree_max_fixed_vars = 0;
};

}  // namespace core::sat::prop

#endif  // EVOL_PARPROP_H
