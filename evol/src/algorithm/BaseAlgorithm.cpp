#include "evol/include/algorithm/BaseAlgorithm.h"

#include <utility>

#include "evol/include/util/Registry.h"

namespace ea::algorithm {

BaseAlgorithm::BaseAlgorithm(
    instance::RPopulation population, generator::RGeneration generator, selector::RSelector selector,
    limit::RLimit limit)
    : population_(std::move(population)),
      generator_(std::move(generator)),
      selector_(std::move(selector)),
      limit_(std::move(limit)) {
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

}  // namespace ea::algorithm

namespace ea::builder {

void BaseAlgorithmBuilder::set_population(instance::RPopulation population) {
  population_ = std::move(population);
}

void BaseAlgorithmBuilder::set_generator(generator::RGeneration generator) {
  generator_ = std::move(generator);
}

void BaseAlgorithmBuilder::set_selector(selector::RSelector selector) {
  selector_ = std::move(selector);
}

void BaseAlgorithmBuilder::set_limit(limit::RLimit limit) {
  limit_ = std::move(limit);
}

algorithm::Algorithm* BaseAlgorithmBuilder::build() {
  return new algorithm::BaseAlgorithm(population_, generator_, selector_, limit_);
}

char const** BaseAlgorithmBuilder::get_params() {
  static char const* params[] = {
      "population",
      "generator",
      "selector",
      "limit",
  };
  return params;
}

/* Marked noexcept explicitly */
REGISTER(ea::algorithm::Algorithm, BaseAlgorithm);

} // namespace ea::builder
