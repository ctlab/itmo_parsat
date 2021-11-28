#include "evol/include/util/random.h"

namespace ea::random {

Generator& Generator::instance() {
  static Generator gen_;
  return gen_;
}

std::mt19937& Generator::stdgen() {
  return mt_;
}

bool flip_coin(double p) {
  return p >= std::uniform_real_distribution<double>(0., 1.)(stdgen());
}

std::mt19937& stdgen() {
  return Generator::instance().stdgen();
}

}  // namespace ea::random