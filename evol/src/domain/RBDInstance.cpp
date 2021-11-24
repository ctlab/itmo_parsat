#include "evol/include/domain/RBDInstance.h"

#include <glog/logging.h>

#include <memory>
#include <utility>

#include "evol/include/util/Registry.h"
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

void kek(
    ::ea::sat::Solver& solver, Minisat::vec<Minisat::Lit> const& assumptions,
    Minisat::vec<Minisat::Lit>& propagated, std::set<int>& collection) {
  solver.propagate(assumptions, propagated);
  for (int j = 0; j < propagated.size(); ++j) {
    collection.insert(var(propagated[(int) j]));
  }
}

}  // namespace

namespace ea::instance {

void RBDInstance::_init_heuristic(RBDInstanceConfig const& config) {
  if (var_map.empty()) {
    Minisat::vec<Minisat::Lit> assumptions(1);
    Minisat::vec<Minisat::Lit> propagated;
    std::vector<std::pair<int, int>> stats;
    stats.reserve(solver_->num_vars());

    for (unsigned i = 0; i < solver_->num_vars(); ++i) {
      std::set<int> prop_both;
      assumptions[0] = Minisat::mkLit((int) i, true);
      kek(*solver_, assumptions, propagated, prop_both);
      assumptions[0] = Minisat::mkLit((int) i, false);
      kek(*solver_, assumptions, propagated, prop_both);
      stats.emplace_back(prop_both.size(), i);
    }

    /* Fill watched variables */
    uint64_t max_watched_count = resolve_limit(
        config.heuristic_config().frac(), config.heuristic_config().count(), stats.size());

    std::sort(stats.begin(), stats.end());
    auto it = stats.crbegin();
    for (size_t i = 0; i < max_watched_count; ++i, ++it) {
      var_map[(int) i] = it->second;
    }
  }

  vars_.resize(var_map.size());
  auto pos = std::uniform_int_distribution<unsigned>(
      0, var_map.size() - 1)(ea::random::Generator::stdgen());
  _flip_var(pos);
}

RBDInstance::RBDInstance(RBDInstanceConfig const& config, sat::RSolver solver)
    : omega_x(config.omega_x()), solver_(std::move(solver)) {
  max_sampling_size_ = config.sampling_config().count();
  switch (config.init()) {
    case RBDInstanceConfig_Init_HEURISTIC:
      _init_heuristic(config);
      break;
    default:
      LOG(FATAL) << "Invalid RBDInstanceConfig.";
  }
}

std::vector<bool> RBDInstance::get_variables() {
  return vars_;
}

Instance* RBDInstance::clone() {
  return new RBDInstance(*this);
}

Fitness const& RBDInstance::fitness() {
  if (cache_state_ == CACHE) {
    return fit_;
  }

  uint64_t success = 0, samples = max_sampling_size_;
  std::unique_ptr<Assignment> assignment_ptr;
  if (std::log2(max_sampling_size_) > vars_size_) {
    samples = (uint32_t) std::pow(2, vars_size_);
    assignment_ptr = std::make_unique<FullSearch>(var_map, vars_);
  } else {
    assignment_ptr = std::make_unique<RandomAssignments>(var_map, vars_);
  }

  Assignment& assignment = *assignment_ptr;
  for (uint64_t i = 0; i < samples; ++i, ++assignment) {
    success += !solver_->propagate(assignment());
  }

  fit_.rho = (double) success / (double) samples;
  fit_.pow_r = vars_size_;
  fit_.pow_nr = omega_x;

  VLOG(7) << "rho=" << fit_.rho << ", fit=" << (double) fit_;

  cache_state_ = CACHE;
  return fit_;
}

RInstance RBDInstance::crossover(RInstance const& other) {
  LOG(FATAL) << "Crossover is not yet implemented for RBDInstance.";
  return nullptr;
}

void RBDInstance::mutate() {
  /* TODO: avoid instance duplication, introduce cache. */
  double prob = 1. / (double) vars_.size();
  for (size_t pos = 0; pos < vars_.size(); ++pos) {
    if (random::flip_coin(prob)) {
      _flip_var(pos);
    }
  }
  cache_state_ = NO_CACHE;
}

void RBDInstance::_flip_var(size_t pos) {
  if (vars_[pos]) {
    --vars_size_;
  } else {
    ++vars_size_;
  }
  vars_[pos] = !vars_[pos];
}

std::shared_ptr<RBDInstance> createRBDInstance(sat::RSolver const& solver) {
  return std::make_shared<RBDInstance>(config::global_config().rbd_instance_config(), solver);
}

}  // namespace ea::instance
