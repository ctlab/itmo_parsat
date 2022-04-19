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

#include "core/sat/prop/SimpProp.h"
#include "core/sat/prop/Prop.h"
#include "util/WorkerPool.h"
#include "util/Random.h"

namespace core::sat::prop {

/**
 * @brief Parallel propagation implementation.
 * @note Intended to be used in parallel environment.
 */
class ParProp : public Prop {
 public:
  explicit ParProp(ParPropConfig const& config);

  std::vector<MinisatSimpBase*> native() noexcept override;

  void load_problem(Problem const& problem) override;

  [[nodiscard]] uint32_t num_vars() const noexcept override;

  bool propagate(lit_vec_t const& assumptions, lit_vec_t& propagated) override;

  bool propagate(lit_vec_t const& assumptions) override;

  uint64_t prop_search(search::USearch search_p, prop_callback_t const& callback) override;

  uint64_t prop_search(
      lit_vec_t const& base_assumption, search::USearch search_p, prop_callback_t const& callback) override;

  uint64_t prop_tree(lit_vec_t const& vars, uint32_t head_size) override;

 private:
  static uint64_t _sum(std::vector<std::future<uint64_t>>& futures) noexcept;

  template <typename... Args>
  uint64_t _prop_search(search::USearch search_p, Prop::prop_callback_t const& callback, Args const&... args) noexcept;

 private:
  using PropWorkerPool = util::WorkerPool<MinisatSimpBase, uint64_t>;

 private:
  uint32_t _tree_max_fixed_vars = 0;
  PropWorkerPool _prop_worker_pool;
};

template <typename... Args>
uint64_t ParProp::_prop_search(
    search::USearch search_p, Prop::prop_callback_t const& callback, Args const&... args) noexcept {
  /* We apply sequential propagation if the number of samples is less than 8192 */
  static constexpr size_t SAMPLES_PARALLEL_MIN = 1ULL << 13;
  static thread_local std::vector<std::future<uint64_t>> futures;
  if (search_p->size() <= SAMPLES_PARALLEL_MIN) {
    return _prop_worker_pool
        .submit([&args..., &search_p, &callback](auto& prop) {
          return sequential_propagate(prop, args..., std::move(search_p), callback);
        })
        .get();
  }
  uint32_t num_threads = std::min(search_p->size() / SAMPLES_PARALLEL_MIN, _prop_worker_pool.max_threads());
  futures.clear();
  futures.reserve(num_threads);
  for (uint32_t index = 0; index < num_threads; ++index) {
    futures.push_back(_prop_worker_pool.submit([&args..., index, &num_threads, &search_p, &callback](auto& prop) {
      return sequential_propagate(prop, args..., search_p->split_search(num_threads, index), callback);
    }));
  }
  return _sum(futures);
}

}  // namespace core::sat::prop

#endif  // EVOL_PARPROP_H
