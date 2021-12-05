#include "evol/include/algorithm/EAAlgorithm.h"

namespace ea::algorithm {

EAAlgorithm::EAAlgorithm(EAAlgorithmConfig const& config)
    : mutator_(ea::registry::Registry::resolve_mutation(config.mutation_type()))
    , selector_(ea::registry::Registry::resolve_selector(config.selector_type())) {}

void EAAlgorithm::prepare() {
  population_.push_back(instance::createInstance(solver_));
}

void EAAlgorithm::step() {
  instance::RInstance child(population_.front()->clone());
  while (instance::Instance::is_cached(child->get_mask())) {
    mutator_->apply(*child);
  }

  population_.push_back(child);
  selector_->select(population_, 1);
}

REGISTER_PROTO(Algorithm, EAAlgorithm, ea_algorithm_config);

}  // namespace ea::algorithm
