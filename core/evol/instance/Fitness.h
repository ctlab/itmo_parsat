#ifndef ITMO_PARSAT_FITNESS_H
#define ITMO_PARSAT_FITNESS_H

#include "core/util/Logger.h"
#include <cstdint>

namespace ea::instance {

/**
 * @brief This structure represents Instances' fitnesses.
 * Mathematically, it is calculated as rho * 2^{pow_r} + (1 - \rho) * 2^{pow_nr}.
 */
struct Fitness {
  double rho;
  int32_t pow_r;
  int32_t pow_nr;
  uint32_t samples;

  /**
   * @return whether value can be calculated using uint64 and double types
   */
  [[nodiscard]] bool can_calc() const;

  /**
   * @return the fitness value in double
   */
  explicit operator double() const;
};

bool operator<(ea::instance::Fitness const& a, ea::instance::Fitness const& b);

}  // namespace ea::instance

#endif  // ITMO_PARSAT_FITNESS_H
