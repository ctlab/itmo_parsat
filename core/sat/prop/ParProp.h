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
 */
class ParProp : public Prop {
 private:
  void _propagate(sat::prop::Prop& prop, domain::USearch search, prop_callback_t const& callback);

 public:
  explicit ParProp(ParPropConfig const& config);

  void load_problem(Problem const& problem) override;

  bool propagate(
      Mini::vec<Mini::Lit> const& assumptions, Mini::vec<Mini::Lit>& propagated) override;

  void prop_assignments(domain::USearch assignment, prop_callback_t const& callback) override;

  uint64_t prop_tree(Mini::vec<Mini::Lit> const& vars, uint32_t head_size) override;

  [[nodiscard]] uint32_t num_vars() const noexcept override;

 private:
  using PropWorkerPool = util::WorkerPool<RProp>;
  PropWorkerPool _prop_worker_pool;
};

}  // namespace core::sat::prop

#endif  // EVOL_PARPROP_H
