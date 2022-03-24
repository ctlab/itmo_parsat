#ifndef EVOL_RANDOM_H
#define EVOL_RANDOM_H

#include <random>
#include <algorithm>
#include "Logger.h"

namespace util::random {

/**
 * @return MTE associated with the current thread.
 */
std::mt19937& stdgen();

/// @brief coin with probability p
bool flip_coin(double p);

/// @brief coin with probability 1 / inv
bool flip_coin(uint32_t inv);

/// @brief coin with probability a / b
bool flip_coin(uint32_t a, uint32_t b);

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

/**
 * @brief In some sense thread-safe and deterministic random engine.
 */
class Generator {
  friend std::mt19937& random::stdgen();

 public:
  /**
   * @brief Creates the generator for the current thread.
   * All methods of this instance can be accessed by functions from
   * ::util::random. This ensures that no thread-unsafe operations will
   * be performed (because generator will be created for each thread).
   *
   * NOTE: each thread that wants to use Generator *must* initialize
   * an instance of this class and keep it alive while it needs its methods.
   */
  explicit Generator(uint32_t seed);

  /**
   * @brief Initializes generator using the seed other generator.
   * Not exactly copying by design.
   * @param other the generator to inherit from.
   */
  Generator(Generator const& other);

  ~Generator() noexcept;

  /**
   * @return Generator assigned to the current thread.
   */
  [[nodiscard]] static Generator& current_thread_generator();

 private:
  uint32_t _seed;
  std::mt19937 _mt;
};

}  // namespace util::random

#endif  // EVOL_RANDOM_H