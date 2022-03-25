#include "core/tests/common/util.h"

namespace common {

core::sat::State to_state(Minisat::lbool b) {
  if (b == Minisat::l_True) {
    return core::sat::SAT;
  } else if (b == Minisat::l_False) {
    return core::sat::UNSAT;
  } else {
    return core::sat::UNKNOWN;
  }
}

}  // namespace common
