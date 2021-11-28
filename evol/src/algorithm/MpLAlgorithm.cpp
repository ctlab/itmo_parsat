#include "evol/include/algorithm/MpLAlgorithm.h"

#include "evol/include/util/random.h"
#include "evol/include/util/Registry.h"

namespace ea::algorithm {

MpLAlgorithm::MpLAlgorithm(MpLAlgorithmConfig const& config)
    : mutator_(registry::Registry::resolve_mutation(config.mutation_type()))
    , cross_(registry::Registry::resolve_crossover(config.crossover_type()))
    , selector_(registry::Registry::resolve_selector(config.selector_type()))
    , mu_(config.mu()), lambda_(config.lambda()) {
}

void MpLAlgorithm::prepare() {
  population_.reserve(mu_);
  for (uint32_t i = 0; i < mu_; ++i) {
    population_.push_back(instance::createInstance(solver_));
  }
}

void MpLAlgorithm::step() {
  for (size_t i = 0; i < lambda_ / 2; ++i) {
    size_t p1 = random::sample<size_t>(0, mu_ - 1);
    size_t p2 = random::sample<size_t>(0, mu_ - 1);

    size_t ip1 = population_.size();
    population_.emplace_back(population_[p1]->clone());
    size_t ip2 = population_.size();
    population_.emplace_back(population_[p2]->clone());

    cross_->apply(population_[ip1], population_[ip2]);
    mutator_->apply(*population_[ip1]);
    mutator_->apply(*population_[ip2]);
  }

  selector_->select(population_, mu_);
}

REGISTER_PROTO(Algorithm, MpLAlgorithm, mpl_algorithm_config);

}  // namespace ea::method