#include "evol/include/method/mutate/UniformMutation.h"
#include "evol/include/util/random.h"
#include "evol/include/util/Registry.h"

namespace ea::method {

UniformMutation::UniformMutation(UniformMutationConfig const& config) : scale_(config.scale()) {}

void UniformMutation::apply(instance::Instance& instance) {
  size_t num_vars = instance::Instance::num_vars();
  double prob = scale_ / (double) num_vars;
  for (size_t pos = 0; pos < num_vars; ++pos) {
    if (random::flip_coin(prob)) {
      instance.flip_var(pos);
    }
  }
}

REGISTER_PROTO(Mutation, UniformMutation, uniform_mutation_config);

}  // namespace ea::method