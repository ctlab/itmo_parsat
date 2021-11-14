#include "evol/include/base_algorithm.h"

namespace ea::algorithm {

void BaseAlgorithmBuilder::set_population(types::RPopulation population) {
  population_ = population;
}

void BaseAlgorithmBuilder::set_generator(generation::RGeneration generator) {
  generator_ = generator;
}

void BaseAlgorithmBuilder::set_selector(selector::RSelector selector) {
  selector_ = selector;
}

void BaseAlgorithmBuilder::set_limit(limit::RLimit limit) { limit_ = limit; }

IAlgorithm* BaseAlgorithmBuilder::build() {
  return new BaseAlgorithm(population_, generator_, selector_, limit_);
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

REGISTER_ALGORITHM(BaseAlgorithm);

}  // namespace ea::algorithm
