#include "core/sat/prop/ParProp.h"

#include <mutex>
#include <algorithm>

#include "util/options.h"
#include "util/stream.h"

namespace core::sat::prop {

ParProp::ParProp(ParPropConfig const& config) : _prop_worker_pool(config.max_threads()) {
  uint32_t num_threads = 1;
  while (num_threads * 2 <= _prop_worker_pool.max_threads()) {
    num_threads <<= 1;
    ++_tree_max_fixed_vars;
  }
}

std::vector<MinisatSimpBase*> ParProp::native() noexcept {
  std::vector<MinisatSimpBase*> _native;
  IPS_VERIFY(loaded);
  for (auto& solver : _prop_worker_pool.get_workers()) {
    IPS_VERIFY(solver.num_vars() == _prop_worker_pool.get_workers()[0].num_vars());
    _native.push_back(&solver);
  }
  return _native;
}

MinisatSimpBase& ParProp::_get_solver() noexcept {
  return _prop_worker_pool.get_workers().front();
}

MinisatSimpBase const& ParProp::_get_solver() const noexcept {
  return _prop_worker_pool.get_workers().front();
}

void ParProp::load_problem(Problem const& problem) {
  auto& workers = _prop_worker_pool.get_workers();
  std::for_each(IPS_EXEC_POLICY, workers.begin(), workers.end(), [&problem](auto& prop) {
    prop.load_problem(problem);
  });
  loaded = true;
}

bool ParProp::propagate(lit_vec_t const& assumptions, lit_vec_t& propagated) {
  return _prop_worker_pool
      .submit([&assumptions, &propagated](auto& solver) {
        return !solver.prop_check(assumptions, propagated);
      })
      .get();
}

bool ParProp::propagate(lit_vec_t const& assumptions) {
  return _prop_worker_pool
      .submit([&assumptions](auto& solver) { return !solver.prop_check(assumptions); })
      .get();
}

uint64_t ParProp::prop_search(search::USearch search_p, Prop::prop_callback_t const& callback) {
  /* parallel operation, take lock */
  std::lock_guard<std::mutex> lg(_solver_m);
  static std::vector<std::future<uint64_t>> futures;
  uint32_t num_threads = _prop_worker_pool.max_threads();
  _futures.clear();
  _futures.reserve(num_threads);
  for (uint32_t index = 0; index < num_threads; ++index) {
    _futures.push_back(
        _prop_worker_pool.submit([index, num_threads, &search_p, &callback](auto& prop) {
          return sequential_propagate(prop, search_p->split_search(num_threads, index), callback);
        }));
  }
  return _sum();
}

uint64_t ParProp::prop_search(
    lit_vec_t const& base_assumption, search::USearch search_p, prop_callback_t const& callback) {
  /* potentially parallel operation, take lock */
  std::lock_guard<std::mutex> lg(_solver_m);
  uint32_t num_threads = _prop_worker_pool.max_threads();
  _futures.clear();
  _futures.reserve(num_threads);
  for (uint32_t index = 0; index < num_threads; ++index) {
    _futures.push_back(_prop_worker_pool.submit(
        [base_assumption, index, num_threads, &search_p, &callback](auto& prop) {
          return sequential_propagate(
              prop, base_assumption, search_p->split_search(num_threads, index), callback);
        }));
  }
  return _sum();
}

uint64_t ParProp::prop_tree(lit_vec_t const& vars, uint32_t head_size) {
  static constexpr int VARS_TREE_SEQ_MIN = 8;
  /* potentially parallel operation, take lock */
  std::lock_guard<std::mutex> lg(_solver_m);
  uint32_t vars_left = vars.size() - head_size;
  if (vars_left <= VARS_TREE_SEQ_MIN) { return _get_solver().prop_check_subtree(vars, head_size); }

  uint32_t may_be_fixed_vars = vars_left - VARS_TREE_SEQ_MIN;
  uint32_t fixed_vars = std::min(_tree_max_fixed_vars, may_be_fixed_vars);
  uint32_t threads = uint32_t(1) << fixed_vars;

  _futures.clear();
  _futures.reserve(threads);
  for (uint32_t thread = 0; thread < threads; ++thread) {
    _futures.push_back(
        _prop_worker_pool.submit([asgn = vars, base_head = head_size, head_size = fixed_vars,
                                  head_asgn = thread](auto& prop) mutable {
          for (uint32_t i = 0; i < head_size; ++i, head_asgn >>= 1) {
            asgn[base_head + i] = Mini::mkLit(Mini::var(asgn[base_head + i]), head_asgn & 1);
          }
          return prop.prop_check_subtree(asgn, base_head + head_size);
        }));
  }
  return _sum();
}

unsigned ParProp::num_vars() const noexcept {
  return _get_solver().nVars();
}

uint64_t ParProp::_sum() noexcept {
  uint64_t result = 0;
  for (auto& future : _futures) { result += future.get(); }
  return result;
}

REGISTER_PROTO(Prop, ParProp, par_prop_config);

}  // namespace core::sat::prop