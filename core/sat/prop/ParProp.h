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

#include "core/util/Generator.h"
#include "core/sat/prop/Prop.h"
#include "core/util/WorkerPool.h"

namespace core::sat::prop {

/**
 * @brief Parallel propagation implementation.
 */
class ParProp : public Prop {
 private:
  void _propagate(sat::prop::Prop& prop, domain::USearch search, prop_callback_t const& callback);

 public:
  explicit ParProp(ParPropConfig const& config);

  void parse_cnf(std::filesystem::path const& input) override;

  bool propagate(
      Minisat::vec<Minisat::Lit> const& assumptions,
      Minisat::vec<Minisat::Lit>& propagated) override;

  void prop_assignments(
      domain::USplittableSearch assignment, prop_callback_t const& callback) override;

  uint64_t prop_tree(Minisat::vec<Minisat::Lit> const& vars, uint32_t head_size) override;

  [[nodiscard]] uint32_t num_vars() const noexcept override;

 private:
  using PropWorkerPool = core::util::WorkerPool<RProp>;
  PropWorkerPool _prop_worker_pool;
};

}  // namespace core::sat::prop

#endif  // EVOL_PARPROP_H
