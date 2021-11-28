#ifndef EVOL_BESTSELECTOR_H
#define EVOL_BESTSELECTOR_H

#include "evol/include/method/Selector.h"
#include "evol/proto/config.pb.h"

namespace ea::method {

class BestSelector : public Selector {
 public:
  BestSelector() = default;

  void select(instance::Population& population, size_t size) override;
};

}  // namespace ea::method

#endif  // EVOL_BESTSELECTOR_H
