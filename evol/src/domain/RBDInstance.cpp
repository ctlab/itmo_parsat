#include "evol/include/domain/RBDInstance.h"

#include <glog/logging.h>

#include <utility>

#include "evol/include/util/Registry.h"
#include "evol/include/util/random.h"

namespace {

void gen_assumption(std::set<unsigned> const& vars, Minisat::vec<Minisat::Lit>& assumption) {
  CHECK_EQ(assumption.size(), (int) vars.size())
      << "Assumptions vector size is not equal to number of variables.";

  std::mt19937& gen = ::ea::random::Generator::stdgen();
  int assump_index = 0;
  for (unsigned var : vars) {
    int sign = std::uniform_int_distribution<int>(0, 1)(gen);
    assumption[assump_index] = Minisat::mkLit((int) var, sign == 1);
    ++assump_index;
  }
}

uint64_t resolve_limit(double frac, uint64_t max_count, uint64_t total) {
  uint64_t max_count_n = std::min(max_count, total);
  auto max_count_f = uint64_t(frac * (double) total);
  return std::min(max_count_n, max_count_f);
}

}  // namespace

namespace ea::instance {

void RBDInstance::assure_initialized() {
  CHECK(solver_ != nullptr) << "Solver must be specified at the moment of deferred initialization.";

  if (deferred_init_heuristic_) {
    LOG(INFO) << "Started deferred heuristic initialization.";
    Minisat::vec<Minisat::Lit> assumptions(1);
    Minisat::vec<Minisat::Lit> propagated;
    std::vector<std::pair<int, int>> stats;
    stats.reserve(solver_->num_vars());

    for (unsigned i = 0; i < solver_->num_vars(); ++i) {
      std::set<Minisat::Lit> prop_both;

      assumptions[0] = Minisat::mkLit((int) i, true);
      solver_->propagate(assumptions, propagated);
      for (int j = 0; j < propagated.size(); ++j) {
        prop_both.insert(propagated[(int) i]);
      }

      assumptions[0] = Minisat::mkLit((int) i, false);
      solver_->propagate(assumptions, propagated);
      for (int j = 0; j < propagated.size(); ++j) {
        prop_both.insert(propagated[(int) i]);
      }

      stats.emplace_back(prop_both.size(), i);
    }

    std::sort(stats.begin(), stats.end());
    uint64_t max_count = resolve_limit(
        config_.heuristic_config().frac(), config_.heuristic_config().count(),
        stats.size());
    auto it = stats.crbegin();
    for (size_t i = 0; i < max_count; ++i, ++it) {
      vars_.insert(it->second);
    }

    LOG(INFO) << "Heuristic initialization resulted in " << vars_.size() << " variables.";
  }

  if (deferred_init_random_) {
    LOG(INFO) << "Started deferred random initialization.";
    double prob = 1. / solver_->num_vars();
    for (unsigned i = 0; i < solver_->num_vars(); ++i) {
      if (random::flip_coin(prob)) {
        vars_.insert(i);
      }
    }
  }

  max_sampling_size_ = resolve_limit(
    config_.sampling_config().frac(),
    config_.sampling_config().count(),
    solver_->num_vars()
  );

  deferred_init_random_ = deferred_init_heuristic_ = false;
}

RBDInstance::RBDInstance(RBDInstanceConfig const& config)
    : config_(config) {
  switch (config.init()) {
    case RBDInstanceConfig_Init_HEURISTIC:
      deferred_init_heuristic_ = true;
      break;
    case RBDInstanceConfig_Init_RANDOM:
      deferred_init_random_ = true;
      break;
    default:
      LOG(FATAL) << "Invalid RBDInstanceConfig.";
  }
}

void RBDInstance::set_solver(sat::RSolver const& solver) {
  solver_ = solver;
}

void RBDInstance::get_assumptions(Minisat::vec<Minisat::Lit>& assumptions) {
  assure_initialized();
  for (unsigned var : vars_) {
    int sign = std::uniform_int_distribution<int>(0, 1)(random::Generator::stdgen());
    assumptions.push(Minisat::mkLit((int) var, sign == 1));
  }
}

Instance* RBDInstance::clone() {
  assure_initialized();
  return new RBDInstance(*this);
}

double RBDInstance::fitness() {
  assure_initialized();

  if (cache_state_ == CACHE) {
    LOG(INFO) << "fitness is cached.";
  }
  CHECK_GT(vars_.size(), (size_t) 0) << "Empty backdoor is not allowed.";

  /* Estimate rho */
  uint64_t success = 0;
  Minisat::vec<Minisat::Lit> assumptions((int) vars_.size());
  for (uint64_t i = 0; i < max_sampling_size_; ++i) {
    gen_assumption(vars_, assumptions);
    success += solver_->propagate(assumptions);
  }
  double rho = (double) success / (double) max_sampling_size_;

  /* Calculate fitness */
  /* TODO: max_samples is a stub. Cannot calculate such large values! */
  cached_ = rho * std::pow(2., vars_.size()) +
      (1. - rho) * std::pow(2., config_.omega() * (double) max_sampling_size_);

  LOG(INFO) << "Fitness: " << cached_ << ", Rho: " << rho;
  CHECK(!std::isnan(cached_)) << "NaN fitness, abort.";

  cache_state_ = CACHE;
  return cached_;
}

RInstance RBDInstance::crossover(RInstance const& other) {
  assure_initialized();
  LOG(FATAL) << "Crossover is not yet implemented for RBDInstance.";
  return nullptr;
}

void RBDInstance::mutate() {
  assure_initialized();
  if ((int64_t) vars_.size() < config_.size_config().count()) {
    /* Flip one random variable (for now: further other options will be implemented) */
    unsigned num_vars = solver_->num_vars();
    unsigned var =
        std::uniform_int_distribution<unsigned>(0, num_vars - 1)(random::Generator::stdgen());

    if (vars_.count(var)) {
      vars_.erase(var);
    } else {
      vars_.insert(var);
    }
  } else {
    /* Size reached max, need to remove variable as a mutation */
    unsigned lowest = *vars_.begin();
    unsigned highest = *vars_.end();
    unsigned close_to_remove =
        std::uniform_int_distribution<unsigned>(lowest, highest)(random::Generator::stdgen());
    vars_.erase(vars_.lower_bound(close_to_remove));
  }

  cache_state_ = NO_CACHE;
}

REGISTER_PROTO(Instance, RBDInstance, rbd_instance_config);

}  // namespace ea::instance