#include "core/evol/limit/FitnessLimit.h"

#include "core/evol/algorithm/Algorithm.h"

namespace ea::limit {

FitnessLimit::FitnessLimit(FitnessLimitConfig const& config)
    : _lowest_fitness(config.lowest_fitness()), _max_size(config.max_size()) {}

bool FitnessLimit::_proceed(ea::algorithm::Algorithm& algorithm) {
  auto const& fitness = algorithm.get_best().fitness();
  auto rho_value = fitness.rho;
  auto size = fitness.size;
  bool stop = rho_value >= _lowest_fitness && size <= _max_size;
  IPS_INFO_IF(
      stop, "Fitness exceeded " << _lowest_fitness << " having rho value of "
                                << rho_value << " and size " << size);
  return !stop;
}

REGISTER_PROTO(Limit, FitnessLimit, fitness_limit_config);

}  // namespace ea::limit