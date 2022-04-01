#include "core/sat/prop/ParProp.h"

#include <mutex>
#include <algorithm>

#include "util/options.h"
#include "util/stream.h"

namespace core::sat::prop {

ParProp::ParProp(ParPropConfig const& config)
    : _prop_worker_pool(config.max_threads(), [&config] {
      return RProp(PropRegistry::resolve(config.prop_config()));
    }) {}

void ParProp::load_problem(Problem const& problem) {
  auto& workers = _prop_worker_pool.get_workers();
  std::for_each(
      IPS_EXEC_POLICY, workers.begin(), workers.end(),
      [&problem](auto& prop) { prop->load_problem(problem); });
}

bool ParProp::_propagate(lit_vec_t const& assumptions, lit_vec_t& propagated) {
  return _prop_worker_pool.get_workers().front()->propagate(
      assumptions, propagated);
}

void ParProp::_prop_assignments(
    domain::USearch search_p, Prop::prop_callback_t const& callback) {
  uint32_t num_threads = _prop_worker_pool.max_threads();
  std::vector<std::future<void>> futures;
  futures.reserve(num_threads);
  for (uint32_t index = 0; index < num_threads; ++index) {
    futures.push_back(_prop_worker_pool.submit(
        [index, num_threads, &search_p, &callback](RProp& prop) {
          sequential_propagate(
              *prop, search_p->split_search(num_threads, index), callback);
        }));
  }
  util::wait_for_futures(futures);
}

void ParProp::_prop_assignments(
    lit_vec_t const& base_assumption, domain::USearch search_p,
    prop_callback_t const& callback) {
  uint32_t num_threads = _prop_worker_pool.max_threads();
  std::vector<std::future<void>> futures;
  futures.reserve(num_threads);
  for (uint32_t index = 0; index < num_threads; ++index) {
    futures.push_back(
        _prop_worker_pool.submit([base_assumption, index, num_threads,
                                  &search_p, &callback](RProp& prop) {
          sequential_propagate(
              base_assumption, *prop,
              search_p->split_search(num_threads, index), callback);
        }));
  }
  util::wait_for_futures(futures);
}

uint64_t ParProp::_prop_tree(lit_vec_t const& vars, uint32_t head_size) {
  if (vars.size() - head_size <= 8) {
    // Use single thread to process small requests.
    return _prop_worker_pool.get_workers().front()->prop_tree(vars, head_size);
  }
  // We use the number of threads as the highest power of 2, lower than pool
  // size.
  uint32_t max_threads = 1;
  while (max_threads * 2 < _prop_worker_pool.max_threads()) {
    max_threads <<= 1;
  }

  // first hs vars are fixed, vl = |vars| - hs are left
  // we choose new head size, hs' <= vl
  // the number of threads will be 2^{hs'}
  // threads <= max_threads
  // threads = 2^{hs'} <= 2^{vl}
  uint32_t vars_left = vars.size() - head_size;
  uint32_t threads = std::min(uint32_t(1) << vars_left, max_threads);
  auto new_head_size = (uint32_t) std::log2(threads);
  std::atomic_uint64_t result = 0;
  std::vector<std::future<void>> futures;
  for (uint32_t thread = 0; thread < threads; ++thread) {
    futures.push_back(_prop_worker_pool.submit(
        [asgn = vars, base_head = head_size, head_size = new_head_size,
         head_asgn = thread, &result](RProp& prop) mutable {
          for (uint32_t i = 0; i < head_size; ++i) {
            asgn[base_head + i] = Mini::mkLit(
                Mini::var(asgn[base_head + i]), head_asgn & (1ULL << i));
          }
          result += prop->prop_tree(asgn, base_head + head_size);
        }));
  }
  util::wait_for_futures(futures);
  return result;
}

unsigned ParProp::num_vars() const noexcept {
  return _prop_worker_pool.get_workers().front()->num_vars();
}

REGISTER_PROTO(Prop, ParProp, par_prop_config);

}  // namespace core::sat::prop