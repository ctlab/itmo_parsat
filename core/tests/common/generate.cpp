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

std::vector<int> gen_vars(size_t size, int num_vars) {
  std::vector<int> vars(size);
  for (int& x : vars) {
    x = util::random::sample<int>(0, num_vars - 1);
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

void pass_assumptions(
    std::function<void(core::lit_vec_t&)> const& f, int min_size, int max_size,
    int max_var, int num_tests) {
  while (num_tests--) {
    auto assumption = gen_assumption(min_size, max_size, max_var);
    f(assumption);
  }
}

}  // namespace common