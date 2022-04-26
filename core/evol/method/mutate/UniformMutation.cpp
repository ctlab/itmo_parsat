#include "core/evol/method/mutate/UniformMutation.h"

#include "util/Random.h"

namespace ea::method {

UniformMutation::UniformMutation(UniformMutationConfig const& config) : scale_(config.scale()) {}

void UniformMutation::apply(instance::Instance& instance) {
  uint32_t num_vars = instance.num_vars();
  for (uint32_t i = 0; i < num_vars; ++i) {
    if (util::random::flip_coin(scale_, num_vars)) {
      instance.get_vars().flip(i);
    }
  }
}

REGISTER_PROTO(Mutation, UniformMutation, uniform_mutation_config);

}  // namespace ea::method