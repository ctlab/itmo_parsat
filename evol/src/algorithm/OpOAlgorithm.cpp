#include "evol/include/algorithm/OpOAlgorithm.h"

namespace ea::algorithm {

OpOAlgorithm::OpOAlgorithm(OpOAlgorithmConfig const& config)
    : mutator_(ea::registry::Registry::resolve_mutation(config.mutation_type()))
    , selector_(ea::registry::Registry::resolve_selector(config.selector_type())) {
}

void OpOAlgorithm::prepare() {
  population_.push_back(instance::createInstance(solver_));
}

void OpOAlgorithm::step() {
  instance::RInstance const& parent = population_.front();
  instance::RInstance child(parent->clone());
  mutator_->apply(*child);
  population_.push_back(child);
  selector_->select(population_, 1);
}

REGISTER_PROTO(Algorithm, OpOAlgorithm, opo_algorithm_config);

}  // namespace ea::method
