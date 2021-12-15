#ifndef EVOL_FITNESSLIMIT_H
#define EVOL_FITNESSLIMIT_H

#include "core/evol/limit/Limit.h"

namespace ea::limit {

class FitnessLimit : public Limit {
 public:
  explicit FitnessLimit(const FitnessLimitConfig& config);

  /**
   * Proceeds if the best instance in population has fitness
   * higher than the specified value.
   */
  bool proceed(instance::Population const& population) override;

 private:
  double lowest_fitness_;
};

}  // namespace ea::limit

#endif  // EVOL_FITNESSLIMIT_H
