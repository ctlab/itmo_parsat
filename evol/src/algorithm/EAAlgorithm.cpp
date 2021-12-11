#include "evol/include/algorithm/EAAlgorithm.h"

namespace ea::algorithm {

EAAlgorithm::EAAlgorithm(EAAlgorithmConfig const& config)
    : mutator_(ea::registry::Registry::resolve_mutation(config.mutation_type()))
    , selector_(ea::registry::Registry::resolve_selector(config.selector_type())) {}

void EAAlgorithm::_prepare() {
  _add_instance();
}

void EAAlgorithm::step() {
  domain::RInstance child(population_.front()->clone());
  mutator_->apply(*child);
  while (child->is_cached()) {
    mutator_->apply(*child);
  }

  population_.push_back(child);
  selector_->select(population_, 1);
}

REGISTER_PROTO(Algorithm, EAAlgorithm, ea_algorithm_config);

}  // namespace ea::algorithm
