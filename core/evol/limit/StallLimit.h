#ifndef ITMO_PARSAT_STALLLIMIT_H
#define ITMO_PARSAT_STALLLIMIT_H

#include "core/evol/limit/Limit.h"

namespace ea::limit {

class StallLimit : public Limit {
 public:
  explicit StallLimit(StallLimitConfig const& config);

 protected:
  /**
   * @return iff the stall is not happening
   */
  bool _proceed(ea::algorithm::Algorithm& algorithm) override;

 private:
  uint32_t _steps_before_stall;
  uint32_t _steps_already = 0;
  uint32_t _last_size = 0;
  double _last_rho = 0.;
};

}  // namespace ea::limit

#endif  // ITMO_PARSAT_STALLLIMIT_H