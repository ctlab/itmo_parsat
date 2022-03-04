#include "core/evol/algorithm/Algorithm.h"

#include "util/Logger.h"
#include "util/stream.h"

namespace {

void collect_stats(
    ::core::sat::prop::Prop& prop, Mini::vec<Mini::Lit> const& assumptions,
    Mini::vec<Mini::Lit>& propagated, std::set<int>& collection) {
  (void) prop.propagate(assumptions, propagated);
  for (int j = 0; j < propagated.size(); ++j) {
    collection.insert(var(propagated[(int) j]));
  }
}

}  // namespace

namespace ea::algorithm {

bool Algorithm::_init_shared_data(InstanceConfig const& config) {
  _shared_data = std::make_shared<instance::SharedData>();
  _shared_data->omega_x = config.omega_x();
  _shared_data->cache.set_max_size(config.max_cache_size());
  _shared_data->sampling_config.base_samples = config.sampling_config().base_count();
  _shared_data->sampling_config.scale = config.sampling_config().scale();
  _shared_data->sampling_config.max_scale_steps = config.sampling_config().max_steps();
  if (_prop->num_vars() == 0 || _preprocess->var_view().size() == 0) {
    IPS_WARNING("Algorithm will not proceed because best prop is 1.");
    return false;
  }
  _shared_data->search_space.reset(_preprocess->var_view().size());
  return true;
}

void Algorithm::_add_instance() {
  _population.push_back(std::make_shared<instance::Instance>(_prop, _shared_data, _preprocess));
}

Algorithm::Algorithm(BaseAlgorithmConfig const& config)
    : _instance_config(config.instance_config())
    , _prop(core::sat::prop::PropRegistry::resolve(config.prop_config()))
    , _limit(limit::LimitRegistry::resolve(config.limit_config())) {}

bool Algorithm::prepare(preprocess::RPreprocess const& preprocess) {
  _preprocess = preprocess;
  if (!_init_shared_data(_instance_config)) {
    return false;
  } else {
    _prepare();
    return true;
  }
}

void Algorithm::_prepare() {}

void Algorithm::process() {
  uint64_t iteration = 0;
  _limit->start();
  do {
    IPS_TRACE(step());
    IPS_INFO_T(
        BEST_INSTANCE, "[Thread " << std::hash<std::thread::id>()(std::this_thread::get_id()) % 100
                                  << "] [Iter " << iteration << "]"
                                  << "[Points " << inaccurate_points() << "]"
                                  << " Best instance: " << get_best());
    ++iteration;
  } while (!is_interrupted() && _limit->proceed(*this));
}

void Algorithm::interrupt() {
  IPS_INFO_IF(!_interrupted, "Algorithm has been interrupted.");
  _interrupted = true;
}

bool Algorithm::is_interrupted() const {
  return _interrupted;
}

core::sat::prop::Prop& Algorithm::get_prop() noexcept {
  return *_prop;
}

instance::Instance& Algorithm::get_best() noexcept {
  return *(*std::min_element(
      _population.begin(), _population.end(), [](auto& a, auto& b) { return *a < *b; }));
}

instance::SharedData& Algorithm::get_shared_data() noexcept {
  return *_shared_data;
}

uint64_t Algorithm::inaccurate_points() const noexcept {
  return _shared_data->search_space.inaccurate_visited_points;
}

bool Algorithm::has_unvisited_points() const noexcept {
  return _shared_data->search_space.has_unvisited_points();
}

}  // namespace ea::algorithm
