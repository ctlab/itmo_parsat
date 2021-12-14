#ifndef EVOL_MUTATION_H
#define EVOL_MUTATION_H

#include "core/util/Registry.h"
#include "evol/instance/Instance.h"
#include "core/proto/config.pb.h"

namespace ea::method {

class Mutation {
 public:
  virtual ~Mutation() = default;

  virtual void apply(instance::Instance& instance) = 0;
};

using RMutation = std::shared_ptr<Mutation>;

DEFINE_REGISTRY(Mutation, MutationConfig, mutation);

}  // namespace ea::method

#endif  // EVOL_MUTATION_H
