#include "core/evol/algorithm/EAAlgorithm.h"

namespace ea::algorithm {

EAAlgorithm::EAAlgorithm(
    EAAlgorithmConfig const& config, core::sat::prop::RProp prop)
    : Algorithm(config.base_algorithm_config(), std::move(prop))
    , mutator_(method::MutationRegistry::resolve(config.mutation_config()))
    , selector_(method::SelectorRegistry::resolve(config.selector_config())) {}

void EAAlgorithm::_prepare() {
  _add_instance();
}

void EAAlgorithm::_step() {
  instance::RInstance child(_population.front()->clone());
  mutator_->apply(*child);
  while (child->is_cached()) {
    mutator_->apply(*child);
  }
  _population.push_back(child);
  selector_->select(_population, 1);
}

REGISTER_PROTO(Algorithm, EAAlgorithm, ea_algorithm_config);

}  // namespace ea::algorithm
