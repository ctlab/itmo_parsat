#ifndef EVOL_MUTATION_H
#define EVOL_MUTATION_H

#include "core/util/Registry.h"
#include "core/evol/instance/Instance.h"
#include "core/proto/solve_config.pb.h"

namespace ea::method {

/**
 * @brief Mutation interface.
 */
class Mutation {
 public:
  virtual ~Mutation() = default;

  /**
   * @brief Applies mutation operation to the specified instance.
   * @param instance the instance to be mutated.
   */
  virtual void apply(instance::Instance& instance) = 0;
};

using RMutation = std::shared_ptr<Mutation>;

DEFINE_REGISTRY(Mutation, MutationConfig, mutation);

}  // namespace ea::method

#endif  // EVOL_MUTATION_H
