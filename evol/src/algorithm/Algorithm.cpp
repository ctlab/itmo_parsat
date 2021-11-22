#include "evol/include/algorithm/Algorithm.h"

namespace ea::algorithm {

void Algorithm::interrupt() {
  interrupted_ = true;
}

bool Algorithm::is_interrupted() const {
  return interrupted_;
}

}  // namespace ea::algorithm