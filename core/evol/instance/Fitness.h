#ifndef ITMO_PARSAT_FITNESS_H
#define ITMO_PARSAT_FITNESS_H

#include "util/Logger.h"
#include <cstdint>

namespace ea::instance {

/**
 * @brief This structure represents Instances' fitness.
 * It is calculated as rho * 2^{size} + (1 - \rho) * 2^{pow_nr}
 * if this can be effectively represented in built-in types.
 */
struct Fitness {
  /**
   * @brief The approximated rho-value, i.e. the approximation of
   * proportion of conflict assumptions.
   */
  double rho;

  /**
   * @brief The number of variables in this rho-backdoor set.
   */
  uint32_t size;

  /**
   * @brief The constant used to prefer smaller sets.
   */
  uint32_t pow_nr;

  /**
   * @brief The number of samples included in approximation of the rho-value.
   */
  uint32_t samples;

  /**
   * @return whether value can be calculated using native types.
   */
  [[nodiscard]] bool can_calc() const;

  /**
   * @return the fitness value
   */
  explicit operator double() const;
};

/**
 * @brief Compares two fitness values. If they both can be converted to double,
 * then respective double values are compared. Otherwise, inaccurate comparison
 * implementation will be used.
 * @param a the first fitness to be compared
 * @param b the second fitness to be compared
 * @return whether the first fitness is `better' than the second
 */
bool operator<(ea::instance::Fitness const& a, ea::instance::Fitness const& b);

}  // namespace ea::instance

#endif  // ITMO_PARSAT_FITNESS_H
