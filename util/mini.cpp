#include "util/mini.h"

namespace util {

std::vector<bool> convert_ms_model(int n_vars, Mini::vec<Minisat::lbool> const& ms_model) {
  std::vector<bool> model(n_vars, false);
  for (int i = 0; i < ms_model.size(); ++i) {
    model[i] = ms_model[i] == Minisat::l_True;
  }
  return model;
}

std::vector<int> filter_vars(std::vector<int> const& vars, std::unordered_set<int> const& vars_to_filter) {
  std::vector<int> result;
  result.reserve(vars.size());
  for (auto var : vars) {
    if (vars_to_filter.find(var) == vars_to_filter.end()) {
      result.push_back(var);
    }
  }
  return result;
}

std::unordered_set<int> get_vars(Mini::vec<Mini::Lit> const& assumption) {
  std::unordered_set<int> result;
  for (int i = 0; i < assumption.size(); ++i) {
    result.insert(Mini::var(assumption[i]));
  }
  return result;
}

Mini::vec<Mini::Lit> map_to_mini_vars(std::vector<int> const& vars) {
  Mini::vec<Mini::Lit> m_vars(vars.size());
  for (size_t i = 0; i < vars.size(); ++i) {
    m_vars[i] = Mini::mkLit(vars[i], false);
  }
  return m_vars;
}

Mini::vec<Mini::Lit> concat_unique_vars(Mini::vec<Mini::Lit> a, Mini::vec<Mini::Lit> const& b) {
  std::unordered_set<int> used;
  a.capacity(a.size() + b.size());
  for (int i = 0; i < a.size(); ++i) {
    used.insert(Mini::var(a[i]));
  }
  for (int i = 0; i < b.size(); ++i) {
    if (used.count(Mini::var(b[i])) == 0) {
      used.insert(Mini::var(b[i]));
      a.push(b[i]);
    }
  }
  return a;
}

Mini::vec<Mini::Lit> filter(Mini::vec<Mini::Lit> const& a) {
  std::unordered_set<int> set;
  Mini::vec<Mini::Lit> result;
  result.capacity(a.size());
  for (int i = 0; i < a.size(); ++i) {
    if (set.count(a[i].x) == 0) {
      set.insert(a[i].x);
      result.push(a[i]);
    }
  }
  return result;
}

}  // namespace util
