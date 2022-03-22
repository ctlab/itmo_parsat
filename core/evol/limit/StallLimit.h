#ifndef ITMO_PARSAT_STALLLIMIT_H
#define ITMO_PARSAT_STALLLIMIT_H

#include "core/evol/limit/Limit.h"

namespace ea::limit {

/**
 * @brief This limit controls execution by limiting the number of same instances
 * (by count or by time).
 */
class StallLimit : public Limit {
 public:
  explicit StallLimit(StallLimitConfig const& config);

  void start() override;

 protected:
  /**
   * @return iff the stall is not happening
   */
  bool _proceed(ea::algorithm::Algorithm& algorithm) override;

 private:
  double _duration();

 private:
  core::clock_t::time_point _last_change_timestamp;
  uint32_t _steps_before_stall;
  uint32_t _steps_already = 0;
  uint32_t _last_size = 0;
  double _last_rho = 0.;
  double _time_before_stall = 0;
};

}  // namespace ea::limit

#endif  // ITMO_PARSAT_STALLLIMIT_H
