#include "evol/include/method/mutate/UniformMutation.h"
#include "evol/include/util/random.h"
#include "evol/include/util/Registry.h"

namespace ea::method {

UniformMutation::UniformMutation(UniformMutationConfig const& config) : scale_(config.scale()) {}

void UniformMutation::apply(instance::Instance& instance) {
  size_t num_vars = instance::Instance::num_vars();
  double p = scale_ / (double) num_vars;
  for (size_t i = 0; i < instance::Instance::num_vars(); ++i) {
    if (random::flip_coin(p)) {
      instance.flip_var(i);
    }
  }
}

REGISTER_PROTO(Mutation, UniformMutation, uniform_mutation_config);

}  // namespace ea::method