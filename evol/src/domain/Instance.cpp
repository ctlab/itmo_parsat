#include "evol/include/domain/Instance.h"

#include <glog/logging.h>

#include <memory>
#include <unordered_map>
#include <utility>

#include "evol/include/util/profile.h"
#include "evol/include/util/random.h"

namespace {

uint64_t resolve_limit(double frac, uint64_t max_count, uint64_t total) {
  auto max_count_f = uint64_t(frac * (double) total);
  if (frac == 0) {
    return max_count;
  }
  if (max_count == 0) {
    return max_count_f;
  }
  return std::min(max_count, max_count_f);
}

void collect_stats(
    ::ea::sat::Solver& solver, Minisat::vec<Minisat::Lit> const& assumptions,
    Minisat::vec<Minisat::Lit>& propagated, std::set<int>& collection) {
  solver.propagate(assumptions, propagated);
  for (int j = 0; j < propagated.size(); ++j) {
    collection.insert(var(propagated[(int) j]));
  }
}

const size_t MAX_CACHE_SIZE = 100000;
std::unordered_map<std::vector<bool>, ::ea::instance::Fitness> fit_cache_{};

}  // namespace

namespace ea::instance {

void Instance::Vars::flip(size_t pos) {
  bit_mask[pos] = !bit_mask[pos];
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

    // TODO: use intended solver method (to abuse parallel solver)
    for (unsigned i = 0; i < solver_->num_vars(); ++i) {
      std::set<int> prop_both;
      assumptions[0] = Minisat::mkLit((int) i, true);
      collect_stats(*solver_, assumptions, propagated, prop_both);
      assumptions[0] = Minisat::mkLit((int) i, false);
      collect_stats(*solver_, assumptions, propagated, prop_both);
      stats.emplace_back(prop_both.size(), i);
    }

    uint64_t max_watched_count = resolve_limit(
        config.heuristic_config().frac(), config.heuristic_config().count(), stats.size());
    std::sort(stats.begin(), stats.end());
    auto it = stats.crbegin();
    for (size_t i = 0; i < max_watched_count; ++i, ++it) {
      var_map_[(int) i] = it->second;
    }
  }

  vars_.bit_mask.resize(var_map_.size(), false);
  flip_var(random::sample<unsigned>(0, num_vars()));
}

Instance::Instance(InstanceConfig const& config, sat::RSolver solver)
    : solver_(std::move(solver)), omega_x(config.omega_x()) {
  max_sampling_size_ = config.sampling_config().count();
  switch (config.init()) {
    case InstanceConfig_Init_HEURISTIC:
      _init_heuristic(config);
      break;
    default:
      LOG(FATAL) << "Invalid InstanceConfig.";
  }
}

std::vector<bool> const& Instance::get_variables() const noexcept {
  return vars_.bit_mask;
}

std::vector<bool>& Instance::get_variables() noexcept {
  return vars_.bit_mask;
}

Instance* Instance::clone() {
  return new Instance(*this);
}

Fitness const& Instance::fitness() {
  if (cache_state_ == CACHE) {
    return fit_;
  }
  auto it = fit_cache_.find(get_variables());
  if (it != fit_cache_.end()) {
    VLOG(5) << "Instance-fit cache hit.";
    fit_ = it->second;
  } else {
    _calc_fitness();
    if (fit_cache_.size() == MAX_CACHE_SIZE) {
      /* TODO: more clever removal */
      fit_cache_.erase(fit_cache_.begin());
    }
    fit_cache_[get_variables()] = fit_;
  }
  return fit_;
}

void Instance::_calc_fitness() {
  ++inaccurate_points_;
  uint64_t samples = max_sampling_size_;
  int size = (int) std::count(vars_.bit_mask.begin(), vars_.bit_mask.end(), true);

  std::unique_ptr<domain::Assignment> assignment_ptr;
  domain::UAssignment assignment_p;
  if (std::log2(max_sampling_size_) >= (double) size) {
    samples = (uint32_t) std::pow(2, size);
    assignment_p = std::make_unique<domain::FullSearch>(var_map(), vars_.bit_mask);
  } else {
    assignment_p = std::make_unique<domain::RandomAssignments>(var_map(), vars_.bit_mask, samples);
  }

  std::atomic_int success(0);
  // clang-format off
  solver_->prop_assignments(std::move(assignment_p),
      [&success](bool conflict, auto const&, auto const&) {
        success += conflict;
        return true;
      });
  // clang-format on

  fit_.rho = (double) success / (double) samples;
  fit_.pow_r = size;
  fit_.pow_nr = (int) omega_x;

  VLOG(7) << "num_vars: " << fit_.pow_r << ", rho=" << fit_.rho << ", fit=" << (double) fit_;
  cache_state_ = CACHE;
}

void Instance::flip_var(size_t pos) {
  vars_.flip(pos);
  cache_state_ = NO_CACHE;
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

std::map<int, int> Instance::var_map_{};

std::atomic_uint32_t Instance::inaccurate_points_{0};

}  // namespace ea::instance

namespace ea::instance {

bool Fitness::can_calc() const {
  return pow_r < 64 && pow_nr < 64;
}

Fitness::operator double() const {
  return std::log2(rho * std::pow(2., pow_r) + (1. - rho) * std::pow(2., pow_nr));
}

bool operator<(Fitness const& a, Fitness const& b) {
  if (a.pow_r == 0) {
    return false;
  }
  if (b.pow_r == 0) {
    return true;
  }

  /* NOTE: For now, pow_nr is always equal to 20 */
  if (a.can_calc() && b.can_calc()) {
    return (double) a < (double) b;
  }

  LOG(WARNING) << "Comparison of non-evaluable Fitness-es.";
  int32_t min_pow_r = std::min(a.pow_r, b.pow_r);
  double a_val = a.rho * std::pow(2., a.pow_r - min_pow_r);
  double b_val = b.rho * std::pow(2., b.pow_r - min_pow_r);

  int32_t pow_nr = a.pow_nr - min_pow_r;
  if (min_pow_r > -60) {
    a_val += (1. - a.rho) * std::pow(2., pow_nr);
    b_val += (1. - b.rho) * std::pow(2., pow_nr);
  }

  return a_val < b_val;
}

bool operator<(Instance& a, Instance& b) {
  return a.fitness() < b.fitness();
}

}  // namespace ea::instance
