#include "core/evol/algorithm/GAAlgorithm.h"

#include "core/util/random.h"

namespace {

std::vector<size_t> choose(std::vector<double> const& distrib, size_t count) {
  std::vector<size_t> res(count);
  std::vector<double> cumulative(distrib);
  for (size_t i = 0; i < cumulative.size(); ++i) {
    cumulative[i] = 1. / cumulative[i];
    if (i > 0) {
      cumulative[i] += cumulative[i - 1];
    }
  }
  double sum = cumulative.back();
  std::for_each(cumulative.begin(), cumulative.end(), [sum](double& x) { x /= sum; });

  while (count--) {
    double sample = ::core::random::sample<double>(0., 1.);
    res[count] =
        std::lower_bound(cumulative.begin(), cumulative.end(), sample) - cumulative.begin();
  }

  return res;
}

}  // namespace

namespace ea::algorithm {

GAAlgorithm::GAAlgorithm(GAAlgorithmConfig const& config)
    : Algorithm(config.base_algorithm_config())
    , mutator_(method::MutationRegistry::resolve(config.mutation_config()))
    , cross_(method::CrossoverRegistry::resolve(config.crossover_config()))
    , selector_(method::SelectorRegistry::resolve(config.selector_config()))
    , q_(config.q())
    , h_(config.h()) {}

void GAAlgorithm::_prepare() {
  population_.reserve(q_);
  for (uint32_t i = 0; i < q_; ++i) {
    _add_instance();
    fits_.push_back((double) population_.back()->fitness());
  }
}

void GAAlgorithm::step() {
  size_t g_ = q_ - h_;
  std::vector<size_t> parents = choose(fits_, g_);
  std::vector<instance::RInstance> children;
  children.reserve(g_);

  for (size_t i = 0; i < g_ / 2; ++i) {
    size_t p1 = parents[2 * i];
    size_t p2 = parents[2 * i + 1];

    size_t c1 = children.size();
    children.emplace_back(population_[p1]->clone());
    size_t c2 = children.size();
    children.emplace_back(population_[p2]->clone());

    cross_->apply(*children[c1], *children[c2]);
    mutator_->apply(*children[c1]);
    mutator_->apply(*children[c2]);
  }

  selector_->select(population_, h_);

  // clang-format off
  population_.insert(
      population_.end(),
      std::make_move_iterator(children.begin()), std::make_move_iterator(children.end())
  );
  // clang-format on

  _recalc_fits();
}

void GAAlgorithm::_recalc_fits() {
  for (size_t i = 0; i < population_.size(); ++i) {
    fits_[i] = (double) population_[i]->fitness();
  }
}

REGISTER_PROTO(Algorithm, GAAlgorithm, ga_algorithm_config);

}  // namespace ea::algorithm