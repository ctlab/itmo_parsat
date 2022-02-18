#include "core/evol/instance/Instance.h"

#include <unordered_map>
#include <utility>

#include "util/Logger.h"
#include "util/Generator.h"

namespace ea::instance {

uint32_t Instance::num_vars() const noexcept {
  return _shared->var_view.size();
}

Instance::Instance(core::sat::prop::RProp prop, RSharedData shared_data)
    : _prop(std::move(prop)), _shared(std::move(shared_data)) {
  _vars.resize(_var_view().size());

  // Initialize instance by setting one random variable
  _vars.flip(core::random::sample<unsigned>(0, num_vars() - 1));
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

      // Inaccurate number of points is calculated as number of masks
      // for which cache-miss occurred. That means, this number will not
      // be equal to the actual number of distinct visited points unless
      // the number of visited points is less than maximal size of the cache.
      ++_inaccurate_points();
    }
    _cached = true;
  }
  return fit_;
}

Fitness const& Instance::fitness() const noexcept {
  IPS_VERIFY(is_cached() && bool("Const-fitness called on non-cached Instance"));
  return _cached ? fit_ : fit_ = _shared->cache.get(_vars.get_mask()).value();
}

void Instance::_calc_fitness() {
  // We start from base samples count
  _calc_fitness(_sampling_config().base_samples, _sampling_config().max_scale_steps);
}

void Instance::_calc_fitness(uint32_t samples, uint32_t steps_left) {
  auto const& mask = _vars.get_mask();
  int size = (int) std::count(mask.begin(), mask.end(), true);

  bool full_search = false;
  std::atomic_uint64_t conflicts{0};

  if (std::log2(samples) >= (double) size) {
    full_search = true;
    samples = 1ULL << size;
    core::domain::UFullSearch search = core::domain::createFullSearch(_var_view(), mask);
    // clang-format off

#if 1
    conflicts = _prop->prop_tree((*search)(), 0);
#else
    Minisat::vec<Minisat::Lit> vars = (*search)();
    _prop->prop_assignments(std::move(search),
      [&conflicts](bool conflict, auto&&) {
        conflicts += conflict;
        return true;
    });
    uint64_t tree_conflicts = _prop->prop_tree(vars, 0);
    IPS_VERIFY(tree_conflicts == conflicts);
#endif

    // clang-format on
  } else {
    core::domain::USearch search =
        core::domain::createRandomSearch(_var_view(), mask, samples);
    // clang-format off
    _prop->prop_assignments(std::move(search),
      [&conflicts](bool conflict, auto const& asgn) {
        conflicts += conflict;
        return true;
    });
    // clang-format on
  }

  fit_.rho = (double) conflicts / (double) samples;
  fit_.size = size;
  fit_.pow_nr = (int) _shared->omega_x;
  fit_.samples = samples;
  _cached = true;

  if (!full_search && fit_.rho == 1.0 && steps_left > 0) {
    uint32_t scaled_samples = samples * _sampling_config().scale;
    IPS_INFO_T(
        FITNESS_SCALE, "Fitness reached 1 for sampling size "
                           << samples << ", scaling sampling size to " << scaled_samples
                           << " instance:\n"
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
  return fit_.size <= core::domain::SearchSpace::MAX_VARS_FOR_FULL_SEARCH && fit_.rho == 1.0 &&
         fit_.samples == (1ULL << fit_.size);
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

core::domain::VarView& Instance::_var_view() noexcept {
  return _shared->var_view;
}

bool operator<(Instance& a, Instance& b) {
  return a.fitness() < b.fitness();
}

}  // namespace ea::instance

std::ostream& operator<<(std::ostream& os, ea::instance::Instance const& instance) {
  auto vars = instance.get_vars().map_to_vars(instance._shared->var_view);
  std::sort(vars.begin(), vars.end());

  double coverage = (double) instance.fitness().samples;
  uint32_t num_vars = instance.fitness().size;
  if (num_vars <= core::domain::SearchSpace::MAX_VARS_FOR_FULL_SEARCH) {
    coverage /= (1ULL << num_vars);
  } else {
    coverage = std::pow(2., std::log2(coverage) - num_vars);
  }

  return os << "Confl. ratio: " << instance.fitness().rho << " Size: " << instance.fitness().size
            << " and fitness: " << (double) instance.fitness() << " Vars: " << vars
            << " samples: " << instance.fitness().samples << " coverage: " << 100. * coverage
            << "%";
}
