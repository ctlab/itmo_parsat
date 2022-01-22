#include "core/util/stream.h"

std::ostream& operator<<(std::ostream& os, Minisat::vec<Minisat::Lit> const& vec) {
  os << "[ ";
  for (int i = 0; i < vec.size(); ++i) {
    if (Minisat::sign(vec[i])) {
      os << Minisat::var(vec[i]) << ' ';
    } else {
      os << "-" << Minisat::var(vec[i]) << ' ';
    }
  }
  return os << "]";
}
