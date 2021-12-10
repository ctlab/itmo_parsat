#include "evol/include/method/mutate/UniformMutation.h"
#include "evol/include/util/random.h"
#include "evol/include/util/Registry.h"

namespace ea::method {

UniformMutation::UniformMutation(UniformMutationConfig const& config) : scale_(config.scale()) {}

void UniformMutation::apply(domain::Instance& instance) {
  size_t num_vars = domain::Instance::num_vars();
  double p = scale_ / (double) num_vars;
  for (size_t i = 0; i < domain::Instance::num_vars(); ++i) {
    if (random::flip_coin(p)) {
      instance.get_vars().flip(i);
    }
  }
}

REGISTER_PROTO(Mutation, UniformMutation, uniform_mutation_config);

}  // namespace ea::method