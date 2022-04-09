#include "core/evol/instance/Instance.h"

#include <unordered_map>
#include <utility>

#include "util/stream.h"
#include "util/Logger.h"
#include "util/Random.h"

namespace ea::instance {

uint32_t Instance::num_vars() const noexcept {
  return _preprocess->var_view().size();
}

Instance::Instance(
    core::sat::prop::RProp prop, RSharedData shared_data,
    preprocess::RPreprocess preprocess)
    : _prop(std::move(prop))
    , _shared(std::move(shared_data))
    , _preprocess(std::move(preprocess)) {
  _vars.resize(_var_view().size());
  _vars.flip(util::random::sample<unsigned>(0, num_vars() - 1));
}

core::domain::Vars& Instance::get_vars() noexcept {
  _cached = false;
  return _vars;
}

core::domain::Vars const& Instance::get_vars() const noexcept {
  return _vars;
}

Instance* Instance::clone() {
  return new Instance(*this);
}

Fitness const& Instance::fitness() {
  if (!_cached) {
    auto const& mask = _vars.get_mask();
    auto maybe_fit = _cache().get(mask);
    if (maybe_fit.has_value()) {
      fit_ = maybe_fit.value();
    } else {
      _calc_fitness();
      _cache().add(mask, fit_);
      ++_inaccurate_points();
    }
    _cached = true;
  }
  return fit_;
}

Fitness const& Instance::fitness() const noexcept {
  IPS_VERIFY(
      is_cached() && bool("Const-fitness called on non-cached Instance"));
  return _cached ? fit_ : fit_ = _shared->cache.get(_vars.get_mask()).value();
}

void Instance::_calc_fitness() {
  _calc_fitness(
      _sampling_config().base_samples, _sampling_config().max_scale_steps);
}

void Instance::_calc_fitness(uint64_t samples, uint32_t steps_left) {
  auto const& mask = _vars.get_mask();
  uint32_t size = _vars.size();
  uint64_t conflicts;
  bool full_search = false;

  if (size <= _sampling_config().max_vars_fs) {
    full_search = true;
    samples = 1ULL << size;
    conflicts = _prop->prop_tree(
        util::concat(
            _shared->base_assumption,
            util::map_to_mini_vars(_vars.map_to_vars(_var_view()))),
        _shared->base_assumption.size());
  } else {
    core::search::USearch search;
    if (size <= core::domain::SearchSpace::MAX_VARS_FOR_FULL_SEARCH &&
        (1ULL << size) <= samples) {
      full_search = true;
      samples = 1ULL << size;
      search = core::search::createFullSearch(_var_view(), mask);
    } else {
      search = core::search::createRandomSearch(_var_view(), mask, samples);
    }
    conflicts = _prop->prop_search(_shared->base_assumption, std::move(search));
  }

  fit_.rho = (double) conflicts / (double) samples;
  fit_.size = size;
  fit_.pow_nr = (int) _shared->omega_x;
  fit_.samples = samples;
  _cached = true;

  if (!full_search && fit_.rho == 1.0 && steps_left > 0) {
    auto scaled_samples = uint32_t(samples * _sampling_config().scale);
    IPS_INFO_T(
        FITNESS_SCALE, "Fitness reached 1 for sampling size "
                           << samples << ", scaling sampling size to "
                           << scaled_samples << " instance:\n"
                           << *this);
    _cached = false;
    _calc_fitness(scaled_samples, steps_left - 1);
  } else {
    IPS_INFO_T(CURRENT_INSTANCE, *this);
  }
}

bool Instance::is_cached() const noexcept {
  return _cached || _shared->cache.get(_vars.get_mask()).has_value();
}

bool Instance::is_sbs() const noexcept {
  return fit_.size <= core::domain::SearchSpace::MAX_VARS_FOR_FULL_SEARCH &&
         fit_.rho == 1.0 && fit_.samples == (1ULL << fit_.size);
}

uint32_t Instance::size() const noexcept {
  return _vars.size();
}

instance::SharedData::SamplingConfig& Instance::_sampling_config() noexcept {
  return _shared->sampling_config;
}

core::LRUCache<std::vector<bool>, Fitness>& Instance::_cache() noexcept {
  return _shared->cache;
}

uint64_t& Instance::_inaccurate_points() noexcept {
  return _shared->search_space.inaccurate_visited_points;
}

core::domain::VarView const& Instance::_var_view() const noexcept {
  return _preprocess->var_view();
}

bool operator<(Instance& a, Instance& b) {
  return a.fitness() < b.fitness();
}

}  // namespace ea::instance

std::ostream& operator<<(
    std::ostream& os, ea::instance::Instance const& instance) {
  auto vars = instance.get_vars().map_to_vars(instance._preprocess->var_view());
  std::sort(vars.begin(), vars.end());

  auto coverage = (double) instance.fitness().samples;
  uint32_t num_vars = instance.fitness().size;
  if (num_vars <= core::domain::SearchSpace::MAX_VARS_FOR_FULL_SEARCH) {
    coverage /= (double) (1ULL << num_vars);
  } else {
    coverage = std::pow(2., std::log2(coverage) - num_vars);
  }

  return os << "Confl. ratio: " << instance.fitness().rho
            << " Size: " << instance.fitness().size
            << " Fitness: " << (double) instance.fitness() << " Vars: " << vars
            << " samples: " << instance.fitness().samples
            << " Coverage: " << 100. * coverage << "%";
}
