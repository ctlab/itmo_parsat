#ifndef MINISAT_RBDINSTANCE_H
#define MINISAT_RBDINSTANCE_H

#include "domain/Instance.h"

namespace ea {

class RBDInstance : public ::ea::instance::Instance {
 public:
  ~RBDInstance() noexcept override = default;

  RBDInstance() = default;

  instance::Instance* clone() const override;

  double fitness() const override;

  instance::RInstance crossover(instance::RInstance const& a) const override;

  static instance::RInstance random();

  void mutate();
};

}  // namespace ea

#endif  // MINISAT_RBDINSTANCE_H
