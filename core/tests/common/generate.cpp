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

void iter_assumptions(
    std::function<void(core::lit_vec_t&)> const& f,
    core::domain::VarView const& var_view, int min_size, int max_size,
    int num_tests) {
  while (num_tests--) {
    auto assumption = gen_assumption(var_view, min_size, max_size);
    f(assumption);
  }
}

void iter_inputs(
    std::function<void(core::sat::Problem const&)> const& f,
    ElimSetting elim_setting) {
  std::filesystem::path path = IPS_PROJECT_ROOT "/resources/cnf/common";
  for (char const* input_name : common::inputs) {
    if (elim_setting == DO_ELIM || elim_setting == BOTH) {
      core::sat::Problem problem(path / input_name, true);
      f(problem);
    }
    if (elim_setting == NO_ELIM || elim_setting == BOTH) {
      core::sat::Problem problem(path / input_name, false);
      f(problem);
    }
  }
}

}  // namespace common