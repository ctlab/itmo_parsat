#ifndef EVOL_FITNESSLIMIT_H
#define EVOL_FITNESSLIMIT_H

#include "core/evol/limit/Limit.h"

namespace ea::limit {

class FitnessLimit : public Limit {
 public:
  /**
   * This limit controls execution by comparing best fitness with the specified limit.
   */
  explicit FitnessLimit(FitnessLimitConfig const& config);

 protected:
  /**
   * @return iff the best instance in population has fitness higher than the specified value.
   */
  bool _proceed(ea::algorithm::Algorithm& algorithm) override;

 private:
  double _lowest_fitness;
};

}  // namespace ea::limit

#endif  // EVOL_FITNESSLIMIT_H
