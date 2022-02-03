#ifndef EVOL_FITNESSLIMIT_H
#define EVOL_FITNESSLIMIT_H

#include "core/evol/limit/Limit.h"

namespace ea::limit {

/**
 * @brief This limit controls execution by comparing best fitness with the specified limit.
 */
class FitnessLimit : public Limit {
 public:
  explicit FitnessLimit(FitnessLimitConfig const& config);

 protected:
  /**
   * @return iff the best instance in population has fitness higher than the specified value.
   */
  bool _proceed(ea::algorithm::Algorithm& algorithm) override;

 private:
  double _lowest_fitness;
  uint32_t _max_size;
};

}  // namespace ea::limit

#endif  // EVOL_FITNESSLIMIT_H
