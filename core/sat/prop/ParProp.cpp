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
  std::for_each(IPS_EXEC_POLICY, workers.begin(), workers.end(), [&problem](auto& prop) {
    prop->load_problem(problem);
  });
}

bool ParProp::propagate(
    Minisat::vec<Minisat::Lit> const& assumptions, Minisat::vec<Minisat::Lit>& propagated) {
  return _prop_worker_pool.get_workers().front()->propagate(assumptions, propagated);
}

void ParProp::prop_assignments(domain::USearch search, Prop::prop_callback_t const& callback) {
  uint32_t num_threads = _prop_worker_pool.max_threads();
  std::vector<std::future<void>> futures;
  futures.reserve(num_threads);
  for (uint32_t index = 0; index < num_threads; ++index) {
    futures.push_back(
        _prop_worker_pool.submit([this, index, num_threads, &search, &callback](RProp& prop) {
          _propagate(*prop, search->split_search(num_threads, index), callback);
        }));
  }
  PropWorkerPool::wait_for(futures);
}

uint64_t ParProp::prop_tree(Minisat::vec<Minisat::Lit> const& vars, uint32_t head_size) {
  if (vars.size() - head_size <= 16) {
    // Use single thread to process small requests.
    return _prop_worker_pool.get_workers().front()->prop_tree(vars, head_size);
  }
  // We use the number of threads as the highest power of 2, lower than pool size.
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
  uint32_t new_head_size = std::log2(threads);
  std::atomic_uint64_t result = 0;
  std::vector<std::future<void>> futures;
  for (uint32_t thread = 0; thread < threads; ++thread) {
    futures.push_back(
        _prop_worker_pool.submit([asgn = vars, base_head = head_size, head_size = new_head_size,
                                  head_asgn = thread, &result](RProp& prop) mutable {
          for (uint32_t i = 0; i < head_size; ++i) {
            asgn[base_head + i] =
                Minisat::mkLit(Minisat::var(asgn[base_head + i]), head_asgn & (1ULL << i));
          }
          result += prop->prop_tree(asgn, base_head + head_size);
        }));
  }
  PropWorkerPool::wait_for(futures);
  return result;
}

void ParProp::_propagate(
    sat::prop::Prop& solver, domain::USearch search, prop_callback_t const& callback) {
  START_ASGN_TRACK(search->size());
  if (!search->empty()) {
    auto& assignment = *search;
    do {
      ASGN_TRACK(assignment());
      bool result = solver.propagate(assignment());
      if (!callback(result, assignment())) {
        BREAK_ASGN_TRACK;
        break;
      }
    } while (++assignment);
  }
  END_ASGN_TRACK(false);
}

unsigned ParProp::num_vars() const noexcept {
  return _prop_worker_pool.get_workers().front()->num_vars();
}

REGISTER_PROTO(Prop, ParProp, par_prop_config);

}  // namespace core::sat::prop