#include "evol/include/util/random.h"

namespace ea::random {

Generator& Generator::instance() {
  static Generator gen_;
  return gen_;
}

std::mt19937& Generator::stdgen() {
  return instance().mt_;
}

bool flip_coin(double p) {
  return p < std::uniform_real_distribution<double>(0., 1.)(Generator::stdgen());
}

}  // namespace ea::random