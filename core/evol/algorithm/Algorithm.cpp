#include "core/evol/algorithm/Algorithm.h"

#include "util/Logger.h"
#include "util/stream.h"
#include "core/proto/solve_config.pb.h"

namespace ea::algorithm {

void Algorithm::_init_shared_data(InstanceConfig const& config) {
  _shared_data = std::make_shared<instance::SharedData>();
  _shared_data->omega_x = config.omega_x();
  _shared_data->cache.set_max_size(config.max_cache_size());
  _shared_data->sampling_config.max_vars_fs = config.sampling_config().max_vars_fs();
  _shared_data->sampling_config.base_samples = config.sampling_config().base_count();
  _shared_data->sampling_config.scale = config.sampling_config().scale();
  _shared_data->sampling_config.max_scale_steps = config.sampling_config().max_steps();
  _shared_data->search_space.reset(_preprocess->var_view().size());
}

void Algorithm::_add_instance() {
  _population.push_back(std::make_shared<instance::Instance>(_prop, _shared_data, _preprocess));
}

Algorithm::Algorithm(BaseAlgorithmConfig const& config, core::sat::prop::RProp prop)
    : _instance_config(config.instance_config())
    , _prop(std::move(prop))
    , _limit(limit::LimitRegistry::resolve(config.limit_config()))
    , _save_stats(config.save_stats()) {}

void Algorithm::prepare(preprocess::RPreprocess const& preprocess) {
  _preprocess = preprocess;
  _init_shared_data(_instance_config);
  _prepare();
}

void Algorithm::_prepare() {}

template <typename T>
void dump_var_vec(std::ostream& ofs, std::vector<T> const& vec) {
  for (int v : vec) {
    ofs << v + 1 << ' ';
  }
  ofs << '\n';
}

void Algorithm::set_problem(core::sat::Problem& p) {
  _problem = &p;
}

void Algorithm::process() {
  _limit->start();
  auto start_timestamp = util::clock_t::now();
  int n1 = 10, n2 = 10, n3 = 10;
  do {
    IPS_BLOCK(algorithm_step, _step());
    auto const& best_instance = get_best();
    IPS_INFO_T(
        BEST_INSTANCE, "[Thread " << std::hash<std::thread::id>()(std::this_thread::get_id()) % 128 << "] [Iter "
                                  << _stats.iterations << "]"
                                  << "[Points " << inaccurate_points() << "]"
                                  << " Best instance: " << best_instance);
    ++_stats.iterations;
    if (_save_stats) {
      _stats.fitness.push_back(best_instance.fitness());
    }

    for (auto& instance : _population) {
      const auto& fit = instance->fitness();
      if (fit.rho < 0.3 && fit.size > 6 && n1 > 0) {
        --n1;
        IPS_INFO("[l] rho: " << fit.rho << "\tsize:" << fit.size);
        dump_var_vec(std::cerr, _problem->remap_variables(instance->get_vars().map_to_vars(_preprocess->var_view())));
      } else if (fit.rho > 0.3 && fit.rho < 0.8 && fit.size > 6 && n2 > 0) {
        --n2;
        IPS_INFO("[m] rho: " << fit.rho << "\tsize:" << fit.size);
        dump_var_vec(std::cerr, _problem->remap_variables(instance->get_vars().map_to_vars(_preprocess->var_view())));
      } else if (fit.rho > 0.9 && fit.size > 6 && n3 > 0) {
        --n3;
        IPS_INFO("[h] rho: " << fit.rho << "\tsize:" << fit.size);
        dump_var_vec(std::cerr, _problem->remap_variables(instance->get_vars().map_to_vars(_preprocess->var_view())));
      }
    }
  } while (!is_interrupted() && _limit->proceed(*this));
  auto end_timestamp = util::clock_t::now();
  _stats.duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_timestamp - start_timestamp).count();
}

void Algorithm::interrupt() {
  IPS_INFO_IF(!_interrupted, "Algorithm has been interrupted.");
  _interrupted = true;
}

bool Algorithm::is_interrupted() const { return _interrupted; }

core::sat::prop::Prop& Algorithm::get_prop() noexcept { return *_prop; }

instance::Instance& Algorithm::get_best() noexcept {
  return *(*std::min_element(_population.begin(), _population.end(), [](auto& a, auto& b) { return *a < *b; }));
}

instance::SharedData& Algorithm::get_shared_data() noexcept { return *_shared_data; }

uint64_t Algorithm::inaccurate_points() const noexcept { return _shared_data->search_space.inaccurate_visited_points; }

bool Algorithm::has_unvisited_points() const noexcept { return _shared_data->search_space.has_unvisited_points(); }

void Algorithm::set_base_assumption(core::lit_vec_t const& assumption) noexcept {
  _shared_data->base_assumption = assumption;
}

AlgorithmStatistics const& Algorithm::get_statistics() const noexcept { return _stats; }

}  // namespace ea::algorithm
