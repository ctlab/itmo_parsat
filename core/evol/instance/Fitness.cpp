#include "core/evol/instance/Fitness.h"

#include <cmath>

namespace ea::instance {

bool Fitness::can_calc() const {
  return size < 62 && pow_nr < 62;
}

Fitness::operator double() const {
  return std::log2(
      rho * std::pow(2., size) + (1. - rho) * std::pow(2., pow_nr));
}

bool operator<(Fitness const& a, Fitness const& b) {
  static constexpr double EPS = 1e-8;
  static constexpr uint32_t CRIT_SZ_DIFF = 32;

  if (a.can_calc() && b.can_calc()) {
    return (double) a < (double) b;
  }

  if (a.size == 0 || a.rho < EPS || a.size > b.size + CRIT_SZ_DIFF) {
    return false;
  }
  if (b.size == 0 || b.rho < EPS || b.size > a.size + CRIT_SZ_DIFF) {
    return true;
  }

  int32_t min_pow_r = std::min(a.size, b.size);
  double a_val = a.rho * std::pow(2., a.size - min_pow_r);
  double b_val = b.rho * std::pow(2., b.size - min_pow_r);

  int32_t pow_nr = a.pow_nr - min_pow_r;
  if (pow_nr > -8) {
    a_val += (1. - a.rho) * std::pow(2., pow_nr);
    b_val += (1. - b.rho) * std::pow(2., pow_nr);
  }

  return a_val < b_val;
}

}  // namespace ea::instance