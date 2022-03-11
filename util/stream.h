#ifndef EVOL_STREAM_H
#define EVOL_STREAM_H

#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>

#include "core/sat/native/mini/utils/Lit.h"
#include "core/sat/native/mini/mtl/Vec.h"

template <typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T> const& v) {
  os << "[ ";
  for (auto const& x : v) {
    os << x << ' ';
  }
  return os << "]";
}

template <typename K, typename V>
std::ostream& operator<<(std::ostream& os, std::unordered_map<K, V> const& map) {
  os << "{ ";
  for (auto const& p : map) {
    os << "[ " << p.first << " -> " << p.second << " ] ";
  }
  return os << "}";
}

template <typename K, typename V>
std::ostream& operator<<(std::ostream& os, std::map<K, V> const& map) {
  os << "{ ";
  for (auto const& p : map) {
    os << "[ " << p.first << " -> " << p.second << " ] ";
  }
  return os << "}";
}

std::ostream& operator<<(std::ostream& os, Mini::vec<Mini::Lit> const& vec);

std::ostream& operator<<(std::ostream& os, Mini::vec<Mini::Lit> const& lit);

#endif  // EVOL_STREAM_H
