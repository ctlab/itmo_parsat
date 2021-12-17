#include "core/evol/limit/FitnessLimit.h"

#include "core/evol/algorithm/Algorithm.h"

namespace ea::limit {

FitnessLimit::FitnessLimit(FitnessLimitConfig const& config)
    : _lowest_fitness(config.lowest_fitness()) {}

bool FitnessLimit::proceed(ea::algorithm::Algorithm& algorithm) {
  auto fitness = static_cast<double>(algorithm.get_best().fitness());
  IPS_INFO_IF(
      fitness >= _lowest_fitness,
      "Fitness exceeded " << _lowest_fitness << " having value of " << fitness);
  return fitness < _lowest_fitness;
}

REGISTER_PROTO(Limit, FitnessLimit, fitness_limit_config);

}  // namespace ea::limit