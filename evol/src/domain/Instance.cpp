#include "evol/include/domain/Instance.h"

#include <memory>
#include <unordered_map>
#include <utility>

#include "evol/include/util/Logging.h"
#include "evol/include/util/random.h"

namespace ea::domain {

void Instance::SamplingConfig::do_scale() {
  CHECK_GT(can_scale, 0U) << "do_scale called when scaling is not possible.";
  --can_scale;
  samples = (uint32_t)(samples * scale);
}

size_t Instance::num_vars() const noexcept {
  return var_view().size();
}

Instance::Instance(sat::RSolver solver, RSharedData shared_data)
    : solver_(std::move(solver)), shared_(std::move(shared_data)) {
  vars_.resize(_var_view().size());
  vars_.flip(random::sample<unsigned>(0, num_vars()));
}

Vars& Instance::get_vars() noexcept {
  cached_ = false;
  return vars_;
}

Instance* Instance::clone() {
  return new Instance(*this);
}

Fitness const& Instance::fitness() {
  if (cached_) {
    return fit_;
  }
  auto const& mask = vars_.get_mask();
  auto maybe_fit = _cache().get(mask);
  if (maybe_fit.has_value()) {
    fit_ = maybe_fit.value();
  } else {
    _calc_fitness();
    _cache().add(mask, fit_);
  }
  cached_ = true;
  return fit_;
}

void Instance::_calc_fitness() {
  ++_inaccurate_points();
  auto const& mask = vars_.get_mask();
  int size = (int) std::count(mask.begin(), mask.end(), true);
  std::unique_ptr<domain::Assignment> assignment_ptr;
  uint32_t samples = _sampling_config().samples;
  domain::UAssignment assignment_p;
  if (std::log2(samples) >= (double) size) {
    samples = (uint32_t) std::pow(2UL, size);
    assignment_p = domain::createFullSearch(var_view(), mask);
  } else {
    assignment_p = domain::createRandomSearch(var_view(), mask, samples);
  }

  // clang-format off
  std::atomic_int success(0), total(0);
  solver_->prop_assignments(std::move(assignment_p),
      [&success, &total](bool conflict, auto const& asgn, auto) {
        success += conflict;
        ++total;
        return true;
      });
  // clang-format on

  LOG_IF(WARNING, total != (int) samples)
      << "Internal error: invalid assignments behaviour: " << total << " != " << samples;
  fit_.rho = (double) success / (double) total;
  fit_.pow_r = size;
  fit_.pow_nr = (int) shared_->omega_x;

  if (fit_.rho == 1.0 && _sampling_config().can_scale > 0) {
    LOG(INFO) << "Fitness reached 1, scaling sampling size, invalidating cache.";
    _sampling_config().do_scale();
    _cache().invalidate();
    _calc_fitness();
  } else {
    EALOG(CURR_INSTANCE) << *this;
  }
}

VarView const& Instance::var_view() const noexcept {
  return shared_->var_view;
}

bool Instance::is_cached() const noexcept {
  return cached_ || shared_->cache.get(vars_.get_mask()).has_value();
}

Instance::SamplingConfig& Instance::_sampling_config() noexcept {
  return shared_->sampling_config;
}

Cache<std::vector<bool>, Fitness>& Instance::_cache() noexcept {
  return shared_->cache;
}

uint32_t& Instance::_inaccurate_points() noexcept {
  return shared_->inaccurate_points;
}

VarView& Instance::_var_view() noexcept {
  return shared_->var_view;
}

bool operator<(Instance& a, Instance& b) {
  return a.fitness() < b.fitness();
}

}  // namespace ea::domain

std::ostream& operator<<(std::ostream& os, ea::domain::Instance& instance) {
  auto vars = instance.get_vars().map_to_vars(instance.var_view());
  std::sort(vars.begin(), vars.end());
  return os << "Fit: " << instance.fitness().rho << " Size: " << instance.fitness().pow_r
            << " and fitness: " << (double) instance.fitness() << " Vars: " << vars
            << " sampling size: " << instance._sampling_config().samples;
}
