#include "evol/include/algorithm/BaseAlgorithm.h"

#include <utility>

#include "evol/include/util/Registry.h"

namespace ea::algorithm {

BaseAlgorithm::BaseAlgorithm()
  : BaseAlgorithm(config::Configuration::get_config().base_algorithm_config()) {
}

BaseAlgorithm::BaseAlgorithm(BaseAlgorithmConfig const& config) {
  registry::Registry& r = registry::Registry::instance();
  generator_ = r.resolve_generator(config.generator_type());
  selector_ = r.resolve_selector(config.selector_type());
  limit_ = r.resolve_limit(config.limit_type());
}

void BaseAlgorithm::process() {
  limit_->start();
  while (limit_->proceed()) {
    LOG_TIME(step());
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

