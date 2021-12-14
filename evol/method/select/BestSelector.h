#ifndef EVOL_BESTSELECTOR_H
#define EVOL_BESTSELECTOR_H

#include "evol/method/select/Selector.h"

namespace ea::method {

class BestSelector : public Selector {
 public:
  BestSelector() = default;

  void select(instance::Population& population, size_t size) override;
};

}  // namespace ea::method

#endif  // EVOL_BESTSELECTOR_H
