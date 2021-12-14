#ifndef EVOL_STREAM_H
#define EVOL_STREAM_H

#include <iostream>

template <typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T> const& v) {
  os << "{ ";
  for (auto const& x : v) {
    os << x << ' ';
  }
  return os << "}";
}

#endif  // EVOL_STREAM_H
