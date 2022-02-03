#include "core/evol/method/mutate/DoerrMutation.h"

#include "core/util/Generator.h"

namespace ea::method {

DoerrMutation::DoerrMutation(DoerrMutationConfig const& config) : beta_(config.beta()) {}

double DoerrMutation::_get_alpha(uint32_t size) const {
  uint32_t bound = size / 2 + 1;
  if (bound < 3) {
    return 1.;
  }

  double ll = 0., rr = 0.;
  double p = core::random::sample<double>(0., 1.);
  double c = 0.;

  for (uint32_t i = 1; i < bound; ++i) {
    c += 1. / std::pow(i, beta_);
  }

  for (uint32_t k = 1; k < bound; ++k) {
    ll = rr;
    rr += (1. / (c * std::pow(k, beta_)));
    if (ll <= p && p < rr) {
      return (double) k;
    }
  }

  return (double) bound - 1.;
}

void DoerrMutation::apply(instance::Instance& instance) {
  uint32_t num_vars = instance.num_vars();
  if (alpha_ == -1.) {
    alpha_ = _get_alpha(num_vars);
  }
  double p = alpha_ / (double) num_vars;

  for (uint32_t i = 0; i < num_vars; ++i) {
    if (core::random::flip_coin(p)) {
      instance.get_vars().flip(i);
    }
  }
}

REGISTER_PROTO(Mutation, DoerrMutation, doerr_mutation_config);

}  // namespace ea::method