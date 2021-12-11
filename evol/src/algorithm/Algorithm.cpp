#include "evol/include/algorithm/Algorithm.h"

#include "evol/include/util/Registry.h"
#include "evol/include/util/SigHandler.h"
#include "evol/include/util/stream.h"
#include "evol/include/util/Logging.h"

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

namespace ea::algorithm {

void Algorithm::_init_shared_data(AlgorithmConfig_InstanceConfig const& config) {
  shared_data_ = std::make_shared<domain::Instance::SharedData>();
  shared_data_->omega_x = config.omega_x();
  shared_data_->cache.set_max_size(config.max_cache_size());
  shared_data_->sampling_config.samples = config.sampling_config().base_count();
  shared_data_->sampling_config.scale = config.sampling_config().scale();
  shared_data_->sampling_config.can_scale = config.sampling_config().max_steps();

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

  uint32_t max_watched_count = std::min(config.heuristic_size(), solver_->num_vars());
  std::sort(stats.begin(), stats.end());
  auto it = stats.crbegin();

  for (size_t i = 0; i < max_watched_count; ++i, ++it) {
    shared_data_->var_view.map_var((int) i, it->second);
  }

  if (Logger::should_log(LogType::HEURISTIC_VARS)) {
    std::stringstream ss;
    for (auto iter = stats.crbegin(); iter != stats.crbegin() + (int) max_watched_count; ++iter) {
      ss << "{ prop: " << iter->first << ", var: " << iter->second << " }\n";
    }
    LOG(INFO) << "Heuristic init:\n" << ss.str();
  }
}

void Algorithm::_add_instance() {
  population_.push_back(std::make_shared<domain::Instance>(solver_, shared_data_));
}

Algorithm::Algorithm() : Algorithm(config::Configuration::get_global_config().algorithm_config()) {}

Algorithm::Algorithm(AlgorithmConfig const& config)
    : instance_config_(config.instance_config())
    , solver_(registry::Registry::resolve_solver(config.solver_type()))
    , limit_(registry::Registry::resolve_limit(config.limit_type())) {}

void Algorithm::prepare() {
  _init_shared_data(instance_config_);
  _prepare();
}

void Algorithm::_prepare() {}

void Algorithm::process() {
  size_t iteration = 0;
  limit_->start();
  while (!is_interrupted() && limit_->proceed(population_)) {
    LOG_TIME(step());
    EALOG(BEST_INSTANCE) << "[Iter " << iteration << "]"
                         << "[Points " << inaccurate_points() << "]"
                         << " Best instance: " << get_best();
    ++iteration;
  }
}

void Algorithm::interrupt() {
  interrupted_ = true;
  solver_->interrupt();
}

bool Algorithm::is_interrupted() const {
  return interrupted_;
}

sat::Solver& Algorithm::get_solver() noexcept {
  return *solver_;
}

domain::Population& Algorithm::get_population() noexcept {
  return population_;
}

domain::Instance& Algorithm::get_best() noexcept {
  return *(*std::min_element(
      population_.begin(), population_.end(), [](auto& a, auto& b) { return *a < *b; }));
}

size_t Algorithm::inaccurate_points() const noexcept {
  return shared_data_->inaccurate_points;
}

}  // namespace ea::algorithm
