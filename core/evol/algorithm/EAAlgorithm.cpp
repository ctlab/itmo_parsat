#include "core/evol/algorithm/EAAlgorithm.h"

namespace ea::algorithm {

EAAlgorithm::EAAlgorithm(EAAlgorithmConfig const& config)
    : Algorithm(config.base_algorithm_config())
    , mutator_(method::MutationRegistry::resolve(config.mutation_config()))
    , selector_(method::SelectorRegistry::resolve(config.selector_config())) {}

void EAAlgorithm::_prepare() {
  _add_instance();
}

void EAAlgorithm::step() {
  instance::RInstance child(population_.front()->clone());
  mutator_->apply(*child);
  while (child->is_cached()) {
    mutator_->apply(*child);
  }

  population_.push_back(child);
  selector_->select(population_, 1);
}

REGISTER_PROTO(Algorithm, EAAlgorithm, AlgorithmConfig, ea_algorithm_config);

}  // namespace ea::algorithm
