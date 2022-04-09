#ifndef ITMO_PARSAT_MINI_H
#define ITMO_PARSAT_MINI_H

#include <unordered_set>

#include "core/sat/native/mini/mtl/Vec.h"
#include "core/sat/native/mini/utils/Lit.h"

namespace util {

template <typename T>
Mini::vec<T> concat(Mini::vec<T> a, Mini::vec<T> const& b) {
  a.capacity(a.size() + b.size());
  for (int i = 0; i < b.size(); ++i) {
    a.push(b[i]);
  }
  return a;
}

std::vector<Mini::Lit> to_std(Mini::vec<Mini::Lit> const& mini_vec);

Mini::vec<Mini::Lit> to_mini(std::vector<Mini::Lit> const& std_vec);

Mini::vec<Mini::Lit> map_to_mini_vars(std::vector<int> const& vars);

Mini::vec<Mini::Lit> concat_unique_vars(
    Mini::vec<Mini::Lit> a, Mini::vec<Mini::Lit> const& b);

Mini::vec<Mini::Lit> filter(Mini::vec<Mini::Lit> const& a);

}  // namespace util

#endif  // ITMO_PARSAT_MINI_H
