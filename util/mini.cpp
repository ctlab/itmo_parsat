#include "util/mini.h"

namespace util {

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