#include "core/evol/limit/FitnessLimit.h"

namespace ea::limit {

FitnessLimit::FitnessLimit(FitnessLimitConfig const& config)
    : lowest_fitness_(config.lowest_fitness()) {}

bool FitnessLimit::proceed(instance::Population const& population) {
  auto it = std::max_element(population.begin(), population.end());
  double fitness = (*it)->fitness().rho;
  IPS_INFO_IF(
      fitness >= lowest_fitness_,
      "Fitness exceeded " << lowest_fitness_ << " having value of " << fitness);
  return fitness < lowest_fitness_;
}

REGISTER_PROTO(Limit, FitnessLimit, LimitConfig, fitness_limit_config);

}  // namespace ea::limit