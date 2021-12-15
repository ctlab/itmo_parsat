#ifndef EVOL_CONJLIMIT_H
#define EVOL_CONJLIMIT_H

#include <memory>

#include "core/evol/limit/Limit.h"

namespace ea::limit {

class ConjLimit : public Limit {
 public:
  explicit ConjLimit(const ConjLimitConfig& config);

  bool proceed(instance::Population const& population) override;

  void start() override;

 private:
  std::vector<RLimit> limits_;
};

}  // namespace ea::limit

#endif  // EVOL_CONJLIMIT_H
