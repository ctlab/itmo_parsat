#include "stream.h"

std::ostream& operator<<(std::ostream& os, core::lit_vec_t const& vec) {
  os << "[ ";
  for (int i = 0; i < vec.size(); ++i) {
    if (Mini::sign(vec[i])) {
      os << Mini::var(vec[i]) << ' ';
    } else {
      os << "-" << Mini::var(vec[i]) << ' ';
    }
  }
  return os << "]";
}

std::ostream& operator<<(std::ostream& os, Mini::Lit const& lit) {
  return os << (Mini::sign(lit) ? "!" : "") << Mini::var(lit);
}
