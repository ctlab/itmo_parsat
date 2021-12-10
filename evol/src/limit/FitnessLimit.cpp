#include "evol/include/limit/FitnessLimit.h"

#include <glog/logging.h>

#include "evol/include/util/Registry.h"

namespace ea::limit {

FitnessLimit::FitnessLimit(FitnessLimitConfig const& config)
    : lowest_fitness_(config.lowest_fitness()) {}

bool FitnessLimit::proceed(domain::Population const& population) {
  auto it = std::max_element(population.begin(), population.end());
  double fitness = (*it)->fitness().rho;
  LOG_IF(INFO, fitness >= lowest_fitness_)
      << "Fitness exceeded " << lowest_fitness_ << " having value of " << fitness;
  return fitness < lowest_fitness_;
}

REGISTER_PROTO(Limit, FitnessLimit, fitness_limit_config);

}  // namespace ea::limit