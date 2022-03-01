#include "core/tests/common/inputs.h"

namespace common {

// clang-format off
char const inputs[1][MAX_INPUT_LEN] = {
    "unsat_bubble_vs_pancake_7_6_simp-@3.cnf",
//  "unsat_aim-100-1_6-no-1-@0.cnf",
//  "unsat_dubois22-@0.cnf",
//  "unsat_sgen6_900_100-@2.cnf",
//  "sat_par8-1-c-@1.cnf",
};
// clang-format on

std::vector<int> gen_vars(size_t size, int num_vars) {
  std::vector<int> vars(size);
  for (int& x : vars) {
    x = util::random::sample<int>(0, num_vars - 1);
  }
  return vars;
}

Mini::vec<Mini::Lit> to_mini(std::vector<int> const& vars) {
  Mini::vec<Mini::Lit> v(vars.size());
  for (size_t i = 0; i < vars.size(); ++i) {
    v[i] = Mini::mkLit(vars[i], false);
  }
  return v;
}

}  // namespace common