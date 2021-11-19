#ifndef EVOL_BESTSELECTOR_H
#define EVOL_BESTSELECTOR_H

#include "evol/include/method/Selector.h"

namespace ea::selector {

class BestSelector : public Selector {
 public:
  void select(instance::Population& population) override;
};

}  // namespace ea::selector

#endif  // EVOL_BESTSELECTOR_H
