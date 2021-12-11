#include "evol/include/method/mutate/DoerrMutation.h"

#include <glog/logging.h>

#include "evol/include/util/random.h"
#include "evol/include/util/Registry.h"

namespace ea::method {

DoerrMutation::DoerrMutation(DoerrMutationConfig const& config) : beta_(config.beta()) {}

double DoerrMutation::_get_alpha(size_t size) const {
  size_t bound = size / 2 + 1;
  if (bound < 3) {
    return 1.;
  }

  double ll = 0., rr = 0.;
  double p = random::sample<double>(0., 1.);
  double c = 0.;

  for (size_t i = 1; i < bound; ++i) {
    c += 1. / std::pow(i, beta_);
  }

  for (size_t k = 1; k < bound; ++k) {
    ll = rr;
    rr += (1. / (c * std::pow(k, beta_)));
    if (ll <= p && p < rr) {
      return (double) k;
    }
  }

  return (double) bound - 1.;
}

void DoerrMutation::apply(domain::Instance& instance) {
  size_t num_vars = instance.num_vars();
  if (alpha_ == -1.) {
    alpha_ = _get_alpha(num_vars);
  }
  double p = alpha_ / (double) num_vars;

  for (size_t i = 0; i < num_vars; ++i) {
    if (random::flip_coin(p)) {
      instance.get_vars().flip(i);
    }
  }
}

REGISTER_PROTO(Mutation, DoerrMutation, doerr_mutation_config);

}  // namespace ea::method