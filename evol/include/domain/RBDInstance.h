#ifndef EVOL_RBDINSTANCE_H
#define EVOL_RBDINSTANCE_H

#include "evol/include/domain/Instance.h"

namespace ea::instance {

class RBDInstance : public Instance {
 public:
  [[nodiscard]] Instance* clone() const override;

  [[nodiscard]] double fitness() const override;

  [[nodiscard]] RInstance crossover(RInstance const& other) const override;

  void mutate() override;

 private:
};

}  // namespace ea::instance

#endif  // EVOL_RBDINSTANCE_H
