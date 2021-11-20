#ifndef EVOL_SIGNALLIMIT_H
#define EVOL_SIGNALLIMIT_H

#include "evol/include/limit/Limit.h"
#include "evol/proto/config.pb.h"

namespace ea::limit {

class SignalLimit : public Limit {
 public:
  explicit SignalLimit(SignalLimitConfig config);

  bool proceed(instance::RPopulation population) override;

 private:
  void set_handlers(void (*)(int));

 private:
  SignalLimitConfig cfg_;
};

}  // namespace ea::limit

#endif  // EVOL_SIGNALLIMIT_H
