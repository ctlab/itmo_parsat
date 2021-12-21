#ifndef EVOL_CONJLIMIT_H
#define EVOL_CONJLIMIT_H

#include <memory>

#include "core/evol/limit/Limit.h"

namespace ea::limit {

class ConjLimit : public Limit {
 public:
  /**
   * Conjunction of a set of limits.
   */
  explicit ConjLimit(const ConjLimitConfig& config);

  void start() override;

 protected:
  /**
   * @return true iff *all* of limits returned true
   */
  bool _proceed(ea::algorithm::Algorithm& algorithm) override;

 private:
  std::vector<RLimit> _limits;
};

}  // namespace ea::limit

#endif  // EVOL_CONJLIMIT_H
