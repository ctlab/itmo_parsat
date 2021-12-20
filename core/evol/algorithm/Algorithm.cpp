#include "core/evol/algorithm/Algorithm.h"

#include "core/util/Logger.h"
#include "core/util/stream.h"

namespace {

void collect_stats(
    ::core::sat::Solver& solver, Minisat::vec<Minisat::Lit> const& assumptions,
    Minisat::vec<Minisat::Lit>& propagated, std::set<int>& collection) {
  solver.propagate(assumptions, propagated);
  for (int j = 0; j < propagated.size(); ++j) {
    collection.insert(var(propagated[(int) j]));
  }
}

}  // namespace

namespace ea::algorithm {

void Algorithm::_init_shared_data(InstanceConfig const& config) {
  _shared_data = std::make_shared<instance::SharedData>();
  _shared_data->omega_x = config.omega_x();
  _shared_data->cache.set_max_size(config.max_cache_size());
  _shared_data->sampling_config.base_samples = config.sampling_config().base_count();
  _shared_data->sampling_config.scale = config.sampling_config().scale();
  _shared_data->sampling_config.max_scale_steps = config.sampling_config().max_steps();

  Minisat::vec<Minisat::Lit> assumptions(1);
  Minisat::vec<Minisat::Lit> propagated;
  std::vector<std::pair<int, int>> stats;
  stats.reserve(_solver->num_vars());

  for (unsigned i = 0; i < _solver->num_vars(); ++i) {
    std::set<int> prop_both;
    assumptions[0] = Minisat::mkLit((int) i, true);
    collect_stats(*_solver, assumptions, propagated, prop_both);
    assumptions[0] = Minisat::mkLit((int) i, false);
    collect_stats(*_solver, assumptions, propagated, prop_both);
    stats.emplace_back(prop_both.size(), i);
  }

  uint32_t max_watched_count = std::min(config.heuristic_size(), _solver->num_vars());
  std::sort(stats.begin(), stats.end());
  auto it = stats.crbegin();

  for (size_t i = 0; i < max_watched_count; ++i, ++it) {
    _shared_data->var_view.map_var((int) i, it->second);
  }

  if (core::Logger::should_log(LogType::HEURISTIC_RESULT)) {
    std::stringstream ss;
    for (auto iter = stats.crbegin(); iter != stats.crbegin() + (int) max_watched_count; ++iter) {
      ss << "{ prop: " << iter->first << ", var: " << iter->second << " }\n";
    }
    IPS_INFO("Heuristic init:\n" << ss.str());
  }
}

void Algorithm::_add_instance() {
  _population.push_back(std::make_shared<instance::Instance>(_solver, _shared_data));
}

Algorithm::Algorithm(BaseAlgorithmConfig const& config)
    : _instance_config(config.instance_config())
    , _solver(core::sat::SolverRegistry::resolve(config.solver_config()))
    , _limit(limit::LimitRegistry::resolve(config.limit_config())) {}

void Algorithm::prepare() {
  _init_shared_data(_instance_config);
  _prepare();
}

void Algorithm::_prepare() {}

void Algorithm::process() {
  size_t iteration = 0;
  _limit->start();
  while (!is_interrupted() && _limit->proceed(*this)) {
    IPS_TRACE(step());
    IPS_INFO_T(
        BEST_INSTANCE, "[Thread " << std::hash<std::thread::id>()(std::this_thread::get_id()) % 100
                                  << "] [Iter " << iteration << "]"
                                  << "[Points " << inaccurate_points() << "]"
                                  << " Best instance: " << get_best());
    ++iteration;
  }
}

void Algorithm::interrupt() {
  _interrupted = true;
  _solver->interrupt();
}

bool Algorithm::is_interrupted() const {
  return _interrupted;
}

core::sat::Solver& Algorithm::get_solver() noexcept {
  return *_solver;
}

instance::Population& Algorithm::get_population() noexcept {
  return _population;
}

instance::Instance& Algorithm::get_best() noexcept {
  return *(*std::min_element(
      _population.begin(), _population.end(), [](auto& a, auto& b) { return *a < *b; }));
}

instance::SharedData& Algorithm::get_shared_data() noexcept {
  return *_shared_data;
}

size_t Algorithm::inaccurate_points() const noexcept {
  return _shared_data->inaccurate_points;
}

}  // namespace ea::algorithm
