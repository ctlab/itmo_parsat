#ifndef EVOL_RANDOM_H
#define EVOL_RANDOM_H

#include <random>
#include <algorithm>
#include "core/util/Logger.h"

namespace core {

namespace random {

/**
 * @return MTE associated with the current thread.
 */
std::mt19937& stdgen();

bool flip_coin(double p);

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

}  // namespace random

/**
 * In some sense thread-safe and deterministic random engine.
 */
class Generator {
  friend std::mt19937& random::stdgen();

 public:
  /**
   * Creates the generator for the current thread.
   * All methods of this instance can be accessed by functions from
   * ::core::random. This ensures that no thread-unsafe operations will
   * be performed (because generator will be created for each thread).
   *
   * NOTE: each thread that wants to use Generator *must* initialize
   * an instance of this class and keep it alive while it needs its methods.
   */
  Generator(uint32_t seed);

  /*
   * Initializes generator using the seed of `other'.
   * Not exactly copying by design.
   */
  Generator(Generator const& other);

  ~Generator() noexcept;

  [[nodiscard]] static Generator& current_thread_generator();

 private:
  uint32_t _seed;
  std::mt19937 _mt;
};

}  // namespace core

#endif  // EVOL_RANDOM_H
