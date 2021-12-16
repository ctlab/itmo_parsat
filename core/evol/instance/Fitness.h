#ifndef ITMO_PARSAT_FITNESS_H
#define ITMO_PARSAT_FITNESS_H

#include "core/util/Logger.h"
#include <cstdint>

namespace ea::instance {

struct Fitness {
  double rho;
  int32_t pow_r;
  int32_t pow_nr;

  /* Check whether value can be calculated using uint64 and double types. */
  [[nodiscard]] bool can_calc() const;

  /* Better check if `can_calc` before using. */
  explicit operator double() const;
};

bool operator<(ea::instance::Fitness const& a, ea::instance::Fitness const& b);

}  // namespace ea::instance

#endif  // ITMO_PARSAT_FITNESS_H
