#include "core/sat/prop/ParProp.h"

#include <mutex>
#include <algorithm>

#include "util/options.h"
#include "util/stream.h"

namespace {

uint32_t ilog2(uint64_t x) {
  uint32_t result = 0;
  while (x != 0) {
    ++result;
    x >>= 1;
  }
  return result;
}

}  // namespace

namespace core::sat::prop {

ParProp::ParProp(ParPropConfig const& config)
    : _tree_max_fixed_vars(ilog2(config.max_threads())), _prop_worker_pool(config.max_threads()) {}

std::vector<MinisatSimpBase*> ParProp::native() noexcept {
  std::vector<MinisatSimpBase*> _native;
  for (auto& solver : _prop_worker_pool.get_workers()) {
    _native.push_back(&solver);
  }
  return _native;
}

void ParProp::load_problem(Problem const& problem) {
  auto& workers = _prop_worker_pool.get_workers();
  std::for_each(
      IPS_EXEC_POLICY, workers.begin(), workers.end(), [&problem](auto& prop) { prop.load_problem(problem); });
}

bool ParProp::propagate(lit_vec_t const& assumptions, lit_vec_t& propagated) {
  return _prop_worker_pool
      .submit([&assumptions, &propagated](auto& solver) { return !solver.prop_check(assumptions, propagated); })
      .get();
}

bool ParProp::propagate(lit_vec_t const& assumptions) {
  return _prop_worker_pool.submit([&assumptions](auto& solver) { return !solver.prop_check(assumptions); }).get();
}

uint64_t ParProp::prop_search(search::USearch search_p, Prop::prop_callback_t const& callback) {
  return _prop_search(std::move(search_p), callback);
}

uint64_t ParProp::prop_search(
    lit_vec_t const& base_assumption, search::USearch search_p, prop_callback_t const& callback) {
  return _prop_search(std::move(search_p), callback, base_assumption);
}

uint64_t ParProp::prop_tree(lit_vec_t const& vars, uint32_t head_size) {
  static constexpr int VARS_TREE_SEQ_MIN = 8;
  static thread_local std::vector<std::future<uint64_t>> futures;
  uint32_t vars_left = vars.size() - head_size;
  if (vars_left <= VARS_TREE_SEQ_MIN) {
    return _prop_worker_pool
        .submit([&vars, &head_size](auto& prop) { return prop.prop_check_subtree(vars, head_size); })
        .get();
  }
  uint32_t may_be_fixed_vars = vars_left - VARS_TREE_SEQ_MIN;
  uint32_t fixed_vars = std::min(_tree_max_fixed_vars, may_be_fixed_vars);
  uint32_t threads = uint32_t(1) << fixed_vars;
  futures.clear();
  futures.reserve(threads);
  for (uint32_t thread = 0; thread < threads; ++thread) {
    futures.push_back(_prop_worker_pool.submit(
        [asgn = vars, &base_head = head_size, &head_size = fixed_vars, head_asgn = thread](auto& prop) mutable {
          for (uint32_t i = 0; i < head_size; ++i, head_asgn >>= 1) {
            asgn[base_head + i] = Mini::mkLit(Mini::var(asgn[base_head + i]), head_asgn & 1);
          }
          return prop.prop_check_subtree(asgn, base_head + head_size);
        }));
  }
  return _sum(futures);
}

unsigned ParProp::num_vars() const noexcept {
  return _prop_worker_pool.get_workers().front().num_vars();
}

uint64_t ParProp::_sum(std::vector<std::future<uint64_t>>& futures) noexcept {
  uint64_t result = 0;
  for (auto& future : futures) {
    result += future.get();
  }
  return result;
}

REGISTER_PROTO(Prop, ParProp, par_prop_config);

}  // namespace core::sat::prop