#include "core/evol/method/mutate/DoerrMutation.h"

#include "util/Random.h"

namespace ea::method {

DoerrMutation::DoerrMutation(DoerrMutationConfig const& config) : beta_(config.beta()) {}

uint32_t DoerrMutation::_get_alpha(uint32_t size) const {
  uint32_t bound = size / 2 + 1;
  if (bound < 3) { return 1; }

  double ll = 0., rr = 0.;
  double p = util::random::sample<double>(0., 1.);
  double c = 0.;

  for (uint32_t i = 1; i < bound; ++i) {
    c += 1. / std::pow(i, beta_);
  }

  for (uint32_t k = 1; k < bound; ++k) {
    ll = rr;
    rr += (1. / (c * std::pow(k, beta_)));
    if (ll <= p && p < rr) { return k; }
  }

  return bound - 1.;
}

void DoerrMutation::apply(instance::Instance& instance) {
  uint32_t num_vars = instance.num_vars();
  if (alpha_ == 0) { alpha_ = _get_alpha(num_vars); }

  for (uint32_t i = 0; i < num_vars; ++i) {
    if (util::random::flip_coin(alpha_, num_vars)) { instance.get_vars().flip(i); }
  }
}

REGISTER_PROTO(Mutation, DoerrMutation, doerr_mutation_config);

}  // namespace ea::method