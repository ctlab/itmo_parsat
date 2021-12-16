#include "core/evol/instance/Fitness.h"

#include <cmath>

namespace ea::instance {

bool Fitness::can_calc() const {
  return pow_r < 64 && pow_nr < 64;
}

Fitness::operator double() const {
  return std::log2(rho * std::pow(2., pow_r) + (1. - rho) * std::pow(2., pow_nr));
}

bool operator<(Fitness const& a, Fitness const& b) {
  if (a.pow_r == 0 || a.rho == 0.0) {
    return false;
  }
  if (b.pow_r == 0 || b.rho == 0.0) {
    return true;
  }

  /* NOTE: For now, pow_nr is always equal to 20 */
  if (a.can_calc() && b.can_calc()) {
    return (double) a < (double) b;
  }

  IPS_WARNING("Comparison of non-evaluable Fitness-es.");
  int32_t min_pow_r = std::min(a.pow_r, b.pow_r);
  double a_val = a.rho * std::pow(2., a.pow_r - min_pow_r);
  double b_val = b.rho * std::pow(2., b.pow_r - min_pow_r);

  int32_t pow_nr = a.pow_nr - min_pow_r;
  if (min_pow_r > -60) {
    a_val += (1. - a.rho) * std::pow(2., pow_nr);
    b_val += (1. - b.rho) * std::pow(2., pow_nr);
  }

  return a_val < b_val;
}

}  // namespace ea::instance