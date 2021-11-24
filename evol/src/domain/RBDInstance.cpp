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
  auto max_count_f = uint64_t(frac * (double) total);
  if (frac == 0) {
    return max_count;
  }
  if (max_count == 0) {
    return max_count_f;
  }
  return std::min(max_count, max_count_f);
}

}  // namespace

namespace ea::instance {

void RBDInstance::assure_initialized() {
  CHECK(solver_ != nullptr) << "Solver must be specified at the moment of deferred initialization.";

  if (deferred_init_heuristic_) {
    VLOG(4) << "Started deferred heuristic initialization.";
    deferred_init_heuristic_ = false;
    max_size_ = resolve_limit(
        config_->size_config().frac(), config_->size_config().count(), solver_->num_vars());

    Minisat::vec<Minisat::Lit> assumptions(1);
    Minisat::vec<Minisat::Lit> propagated;
    std::vector<std::pair<int, int>> stats;
    stats.reserve(solver_->num_vars());

    for (unsigned i = 0; i < solver_->num_vars(); ++i) {
      std::set<int> prop_both;

      assumptions[0] = Minisat::mkLit((int) i, true);
      solver_->propagate(assumptions, propagated);
      for (int j = 0; j < propagated.size(); ++j) {
        prop_both.insert(var(propagated[(int) j]));
      }

      assumptions[0] = Minisat::mkLit((int) i, false);
      solver_->propagate(assumptions, propagated);
      for (int j = 0; j < propagated.size(); ++j) {
        prop_both.insert(var(propagated[(int) j]));
      }

      stats.emplace_back(prop_both.size(), i);
    }

    /* Fill watched variables */
    uint64_t max_watched_count = resolve_limit(
        config_->heuristic_config().frac(), config_->heuristic_config().count(), stats.size());
    watched_ = std::make_shared<std::vector<unsigned>>(max_watched_count);
    std::sort(stats.begin(), stats.end());
    auto it = stats.crbegin();
    for (size_t i = 0; i < max_watched_count; ++i, ++it) {
      watched_->push_back(it->second);
    }

    /* Vars cannot be empty. */
    if (vars_.empty()) {
      vars_.insert((*watched_)[std::uniform_int_distribution<unsigned>(
          0, watched_->size() - 1)(ea::random::Generator::stdgen())]);
    }
  }
}

RBDInstance::RBDInstance(RBDInstanceConfig const& config)
    : config_(std::make_shared<RBDInstanceConfig>(config)) {
  switch (config.init()) {
    case RBDInstanceConfig_Init_HEURISTIC:
      deferred_init_heuristic_ = true;
      break;
    default:
      LOG(FATAL) << "Invalid RBDInstanceConfig.";
  }
}

void RBDInstance::set_solver(sat::RSolver const& solver) {
  solver_ = solver;
}

std::set<unsigned> RBDInstance::get_variables() {
  return vars_;
}

Instance* RBDInstance::clone() {
  assure_initialized();
  return new RBDInstance(*this);
}

Fitness const& RBDInstance::fitness() {
  assure_initialized();

  if (cache_state_ == CACHE) {
    return fit_;
  }
  CHECK_GT(vars_.size(), (size_t) 0) << "Empty backdoor is not allowed.";

  VLOG_IF(4, config_->sampling_config().frac() != 0.)
      << "Frac for sampling config is ignored due to possibly too large values.";
  max_sampling_size_ = config_->sampling_config().count();

  /* Estimate rho */
  uint64_t success = 0;
  Minisat::vec<Minisat::Lit> assumptions((int) vars_.size());

  for (auto x : vars_) {
    std::cout << x << ' ';
  }
  std::cout << std::endl;

  uint64_t sampling = std::min(max_sampling_size_, (uint64_t) std::pow(2, (uint64_t) vars_.size()));
  for (uint64_t i = 0; i < sampling; ++i) {
    /* Generate new assumption */
    gen_assumption(vars_, assumptions);
//    for (int j = 0; j < assumptions.size(); ++j) {
//      std::cout << sign(assumptions[j]) << " ";
//    }
//    std::cout << std::endl;
    /* Add 1 to success iff solver found conflicts */
    success += !solver_->propagate(assumptions);
  }

  fit_.rho = (double) success / (double) sampling;
  fit_.pow_r = vars_.size();
  fit_.pow_nr = 20;
//  fit_.pow_nr = uint32_t(config_->omega() * (double) solver_->num_vars());

  VLOG(7) << "rho=" << fit_.rho << ", fit=" << (double) fit_;

  cache_state_ = CACHE;
  return fit_;
}

RInstance RBDInstance::crossover(RInstance const& other) {
  assure_initialized();
  LOG(FATAL) << "Crossover is not yet implemented for RBDInstance.";
  return nullptr;
}

void RBDInstance::mutate() {
  /* TODO: avoid instance duplication, introduce cache. */
  /* NOTE: maybe store instances as bit vectors? */
  assure_initialized();
  std::vector<unsigned> const& w = *watched_;

  if (vars_.size() < max_size_) {
    /* Flip one random variable (further other options will be implemented) */
    unsigned var =
        w[std::uniform_int_distribution<unsigned>(0, w.size() - 1)(random::Generator::stdgen())];

    // TODO
    // mutation (1/n)
    // up_gad

    if (vars_.count(var)) {
      vars_.erase(var);
    } else {
      vars_.insert(var);
    }
  } else {
    /* Size reached max, need to remove variable as a mutation */
    size_t index =
        std::uniform_int_distribution<size_t>(0, vars_.size() - 1)(random::Generator::stdgen());

    auto it = vars_.begin();
    std::advance(it, index);
    vars_.erase(it);
  }

  cache_state_ = NO_CACHE;
}

REGISTER_PROTO(Instance, RBDInstance, rbd_instance_config);

}  // namespace ea::instance
