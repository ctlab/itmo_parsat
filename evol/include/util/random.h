#ifndef EVOL_RANDOM_H
#define EVOL_RANDOM_H

#include <random>
#include <algorithm>

#include "evol/include/config/Configuration.h"

namespace ea::random {

class Generator {
 public:
  Generator();

  static Generator& instance();

  std::mt19937& stdgen();

 private:
  std::mt19937 mt_;
};

bool flip_coin(double p);

std::mt19937& stdgen();

template <typename It>
void shuffle(It first, It last) {
  std::shuffle(first, last, stdgen());
}

template <typename Flt>
std::enable_if_t<std::is_floating_point_v<Flt>, Flt> sample(Flt min, Flt max) {
  return std::uniform_real_distribution<Flt>(min, max)(stdgen());
}

template <typename Int>
std::enable_if_t<std::is_integral_v<Int>, Int> sample(Int min, Int max) {
  return std::uniform_int_distribution<Int>(min, max)(stdgen());
}

}  // namespace ea::random

#endif  // EVOL_RANDOM_H
