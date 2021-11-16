#ifndef EVOL_BESTSELECTOR_H
#define EVOL_BESTSELECTOR_H

#include "evol/include/method/Selector.h"
#include "evol/include/util/Builder.h"

namespace ea::selector {

class BestSelector : public Selector {
 public:
  void select(instance::Population& population) override;
};

}  // namespace ea::selector

namespace ea::builder {

class BestSelectorBuilder : public Builder<selector::Selector> {
 public:
  BestSelectorBuilder() = default;

  selector::Selector* build() override;

  char const** get_params() override;
};

}  // namespace ea::builder



#endif  // EVOL_BESTSELECTOR_H
