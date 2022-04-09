#include "util/mini.h"

namespace util {

std::vector<Mini::Lit> to_std(Mini::vec<Mini::Lit> const& mini_vec) {
  std::vector<Mini::Lit> std_vec(mini_vec.size());
  std::copy(
      mini_vec.data(), mini_vec.data() + mini_vec.size(), std_vec.begin());
  return std_vec;
}

Mini::vec<Mini::Lit> to_mini(std::vector<Mini::Lit> const& std_vec) {
  Mini::vec<Mini::Lit> mini_vec(std_vec.size());
  std::copy(std_vec.begin(), std_vec.end(), mini_vec.data());
  return mini_vec;
}

Mini::vec<Mini::Lit> map_to_mini_vars(std::vector<int> const& vars) {
  Mini::vec<Mini::Lit> m_vars(vars.size());
  for (size_t i = 0; i < vars.size(); ++i) {
    m_vars[i] = Mini::mkLit(vars[i], false);
  }
  return m_vars;
}

Mini::vec<Mini::Lit> concat_unique_vars(
    Mini::vec<Mini::Lit> a, Mini::vec<Mini::Lit> const& b) {
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