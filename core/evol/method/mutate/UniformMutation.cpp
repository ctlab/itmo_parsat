#include "core/evol/method/mutate/UniformMutation.h"

#include "core/util/random.h"

namespace ea::method {

UniformMutation::UniformMutation(UniformMutationConfig const& config) : scale_(config.scale()) {}

void UniformMutation::apply(instance::Instance& instance) {
  size_t num_vars = instance.num_vars();
  double p = scale_ / (double) num_vars;
  for (size_t i = 0; i < num_vars; ++i) {
    if (core::random::flip_coin(p)) {
      instance.get_vars().flip(i);
    }
  }
}

REGISTER_PROTO(Mutation, UniformMutation, uniform_mutation_config);

}  // namespace ea::method