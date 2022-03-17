#ifndef ITMO_PARSAT_MINI_H
#define ITMO_PARSAT_MINI_H

#include "core/sat/native/mini/mtl/Vec.h"

namespace util {

template <typename T>
Mini::vec<T> concat(Mini::vec<T> a, Mini::vec<T> const& b) {
  a.capacity(a.size() + b.size());
  for (int i = 0; i < b.size(); ++i) {
    a.push(b[i]);
  }
  return a;
}

}  // namespace util

#endif  // ITMO_PARSAT_MINI_H
