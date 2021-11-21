#ifndef EVOL_RANDOM_H
#define EVOL_RANDOM_H

#include <random>

namespace ea::random {

class Generator {
 private:
  static Generator& instance();

 public:
  static std::mt19937& stdgen();

 private:
  std::mt19937 mt_;
};

bool flip_coin(double p);

}  // namespace ea::random

#endif  // EVOL_RANDOM_H
