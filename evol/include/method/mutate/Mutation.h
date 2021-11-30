#ifndef EVOL_MUTATION_H
#define EVOL_MUTATION_H

#include "evol/include/domain/Instance.h"

namespace ea::method {

class Mutation {
 public:
  virtual ~Mutation() = default;

  virtual void apply(instance::Instance& instance) = 0;
};

using RMutation = std::shared_ptr<Mutation>;

}  // namespace ea::method

#endif  // EVOL_MUTATION_H
