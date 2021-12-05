#include "evol/include/util/random.h"

#include <glog/logging.h>

namespace ea::random {

Generator::Generator()
  : mt_(config::global_config().rnd_seed()) {
  VLOG(4) << "Set random seed to " << config::global_config().rnd_seed();
}

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