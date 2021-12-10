#include "evol/include/domain/Instance.h"

#include <memory>
#include <unordered_map>
#include <utility>

#include "evol/include/util/Logging.h"
#include "evol/include/util/random.h"

namespace {

void collect_stats(
    ::ea::sat::Solver& solver, Minisat::vec<Minisat::Lit> const& assumptions,
    Minisat::vec<Minisat::Lit>& propagated, std::set<int>& collection) {
  solver.propagate(assumptions, propagated);
  for (int j = 0; j < propagated.size(); ++j) {
    collection.insert(var(propagated[(int) j]));
  }
}

}  // namespace

namespace ea::domain {

Instance::SamplingConfig::SamplingConfig(uint32_t samples, uint32_t can_scale, double scale)
    : samples(samples), can_scale(can_scale), scale(scale) {}

void Instance::SamplingConfig::do_scale() {
  CHECK_GT(can_scale, 0U) << "do_scale called when scaling is not possible.";
  --can_scale;
  samples = (uint32_t) (samples * scale);
}

size_t Instance::num_vars() noexcept {
  return var_map_.size();
}

void Instance::_init_heuristic(InstanceConfig const& config) {
  if (var_map_.empty()) {
    Minisat::vec<Minisat::Lit> assumptions(1);
    Minisat::vec<Minisat::Lit> propagated;
    std::vector<std::pair<int, int>> stats;
    stats.reserve(solver_->num_vars());

    for (unsigned i = 0; i < solver_->num_vars(); ++i) {
      std::set<int> prop_both;
      assumptions[0] = Minisat::mkLit((int) i, true);
      collect_stats(*solver_, assumptions, propagated, prop_both);
      assumptions[0] = Minisat::mkLit((int) i, false);
      collect_stats(*solver_, assumptions, propagated, prop_both);
      stats.emplace_back(prop_both.size(), i);
    }

    uint64_t max_watched_count = config.heuristic_size();
    std::sort(stats.begin(), stats.end());
    auto it = stats.crbegin();
    for (size_t i = 0; i < max_watched_count; ++i, ++it) {
      var_map_[(int) i] = it->second;
    }

    if (Logger::should_log(LogType::HEURISTIC_VARS)) {
      std::stringstream ss;
      for (auto iter = stats.crbegin(); iter != stats.crbegin() + (int) max_watched_count; ++iter) {
        ss << "{ prop: " << iter->first << ", var: " << iter->second << " }\n";
      }
      LOG(INFO) << "Heuristic init:\n" << ss.str();
    }
  }

  vars_.resize(var_map_.size());
  vars_.flip(random::sample<unsigned>(0, num_vars()));
}

Instance::Instance(InstanceConfig const& config, sat::RSolver solver)
    : solver_(std::move(solver))
    , sampling_config_(std::make_shared<SamplingConfig>(
          config.sampling_config().base_count(), config.sampling_config().max_steps(),
          config.sampling_config().scale()))
    , instance_cache_(std::make_shared<cache_t>(config.max_cache_size()))
    , omega_x(config.omega_x()) {
  _init_heuristic(config);
}

Vars const& Instance::get_vars() const noexcept {
  return vars_;
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
  auto maybe_fit = instance_cache_->get(mask);
  if (maybe_fit.has_value()) {
    fit_ = maybe_fit.value();
  } else {
    _calc_fitness();
    instance_cache_->add(mask, fit_);
  }
  cached_ = true;
  return fit_;
}

void Instance::_calc_fitness() {
  ++inaccurate_points_;
  auto const& mask = vars_.get_mask();
  int size = (int) std::count(mask.begin(), mask.end(), true);
  std::unique_ptr<domain::Assignment> assignment_ptr;
  uint32_t samples = sampling_config_->samples;
  domain::UAssignment assignment_p;
  if (std::log2(samples) >= (double) size) {
    samples = (uint32_t) std::pow(2UL, size);
    assignment_p = domain::createFullSearch(var_map(), mask);
  } else {
    assignment_p = domain::createRandomSearch(var_map(), mask, samples);
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
  fit_.pow_nr = (int) omega_x;

  if (fit_.rho == 1.0 && sampling_config_->can_scale > 0) {
    LOG(INFO) << "Fitness reached 1, scaling sampling size, invalidating cache.";
    sampling_config_->do_scale();
    instance_cache_->invalidate();
    _calc_fitness();
  } else {
    EALOG(LogType::CURR_INSTANCE) << *this;
  }
}

RInstance createInstance(sat::RSolver const& solver) {
  return std::make_shared<Instance>(config::global_config().instance_config(), solver);
}

std::map<int, int> const& Instance::var_map() noexcept {
  return var_map_;
}

uint32_t Instance::inaccurate_points() {
  return inaccurate_points_;
}

bool Instance::is_cached() const noexcept {
  return cached_ || instance_cache_->get(vars_.get_mask()).has_value();
}

std::map<int, int> Instance::var_map_{};

std::atomic_uint32_t Instance::inaccurate_points_{0};

bool operator<(Instance& a, Instance& b) {
  return a.fitness() < b.fitness();
}

}  // namespace ea::domain

std::ostream& operator<<(std::ostream& os, ea::domain::Instance& instance) {
  auto vars = instance.get_vars().map_to_vars(ea::domain::Instance::var_map());
  std::sort(vars.begin(), vars.end());
  return os << "Fit: " << instance.fitness().rho << " Size: " << instance.fitness().pow_r
            << " and fitness: " << (double) instance.fitness() << " Vars: " << vars
            << " sampling size: " << instance.sampling_config_->samples;
}
