#include "evol/include/util/random.h"

namespace ea::random {

Generator& Generator::instance() {
  static Generator gen_;
  return gen_;
}

std::mt19937& Generator::stdgen() {
  return instance().mt_;
}

}  // namespace ea::random