#include "core/tests/common/generate.h"

namespace common {

core::lit_vec_t gen_assumption(int min_size, int max_size, int max_var) {
  int size = util::random::sample<int>(min_size, max_size);
  core::lit_vec_t assumption(size);
  for (int i = 0; i < size; ++i) {
    int var = util::random::sample<int>(0, max_var);
    int sign = util::random::sample<int>(0, 1);
    assumption[i] = Mini::mkLit(var, sign);
  }
  return assumption;
}

core::lit_vec_t gen_assumption(
    core::domain::VarView const& var_view, int min_size, int max_size) {
  int max_var = (int) var_view.size() - 1;
  auto assumption = gen_assumption(min_size, max_size, max_var);
  for (int i = 0; i < assumption.size(); ++i) {
    assumption[i] = Mini::mkLit(
        var_view[Mini::var(assumption[i])], Mini::sign(assumption[i]));
  }
  return assumption;
}

std::vector<int> gen_vars(size_t size, int num_vars) {
  std::vector<int> vars(size);
  for (int& x : vars) {
    x = util::random::sample<int>(0, num_vars - 1);
  }
  return vars;
}

std::vector<int> gen_unique_vars(size_t size, int num_vars) {
  assert(num_vars >= size);
  std::set<int> used_vars;
  std::vector<int> vars(size);
  for (int& x : vars) {
    x = util::random::sample<int>(0, num_vars - 1);
    while (used_vars.count(x)) {
      x = util::random::sample<int>(0, num_vars - 1);
    }
    used_vars.insert(x);
  }
  return vars;
}

core::lit_vec_t to_mini(std::vector<int> const& vars) {
  Mini::vec<Mini::Lit> v((int) vars.size());
  for (size_t i = 0; i < vars.size(); ++i) {
    v[(int) i] = Mini::mkLit(vars[i], false);
  }
  return v;
}

void iter_assumptions(
    std::function<void(core::lit_vec_t&)> const& f, int min_size, int max_size,
    int max_var, int num_tests) {
  while (num_tests--) {
    auto assumption = gen_assumption(min_size, max_size, max_var);
    f(assumption);
  }
}

void iter_vars(
    std::function<void(core::vars_set_t&)> const& f, int min_size, int max_size,
    int max_var, int num_tests) {
  while (num_tests--) {
    auto vars =
        gen_vars(util::random::sample<int>(min_size, max_size), max_var);
    f(vars);
  }
}

void iter_assumptions(
    std::function<void(core::lit_vec_t&)> const& f,
    core::domain::VarView const& var_view, int min_size, int max_size,
    int num_tests) {
  while (num_tests--) {
    auto assumption = gen_assumption(var_view, min_size, max_size);
    f(assumption);
  }
}

}  // namespace common