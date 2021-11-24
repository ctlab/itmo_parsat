#include "evol/include/algorithm/BaseAlgorithm.h"

#include <utility>

#include "evol/include/util/Registry.h"
#include "evol/include/util/SigHandler.h"

namespace ea::algorithm {

BaseAlgorithm::BaseAlgorithm()
    : BaseAlgorithm(config::Configuration::get_global_config().base_algorithm_config()) {
}

BaseAlgorithm::BaseAlgorithm(BaseAlgorithmConfig const& config) {
  generator_ = registry::Registry::resolve_generator(config.generator_type());
  selector_ = registry::Registry::resolve_selector(config.selector_type());
  limit_ = registry::Registry::resolve_limit(config.limit_type());
}

void BaseAlgorithm::process() {
  limit_->start();
  while (!is_interrupted() && limit_->proceed(population_)) {
    LOG_TIME(step());
    VLOG(4) << "Best instance fit: " << (double) population_->front()->fitness().rho;
  }
}

void BaseAlgorithm::step() {
  generator_->generate(*population_);
  selector_->select(*population_);
}

void BaseAlgorithm::set_population(instance::RPopulation population) {
  population_ = std::move(population);
}

REGISTER_PROTO(Algorithm, BaseAlgorithm, base_algorithm_config);

}  // namespace ea::algorithm
