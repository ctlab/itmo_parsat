#include "core/solve/rbs/CartesianRBSearch.h"

namespace {

std::vector<Mini::Lit> to_std_assump(core::lit_vec_t const& assumption) {
  std::vector<Mini::Lit> result(assumption.size());
  for (int i = 0; i < assumption.size(); ++i) {
    result[i] = assumption[i];
  }
  return result;
}

}  // namespace

namespace core::solve {

CartesianRBSearch::CartesianRBSearch(
    CartesianRBSearchConfig config, sat::prop::RProp prop,
    ea::preprocess::RPreprocess preprocess)
    : RBSearch(std::move(prop), std::move(preprocess))
    , _cfg(std::move(config)) {}

void CartesianRBSearch::_interrupt_impl() {
  std::lock_guard<std::mutex> lg(_m);
  for (auto& algorithm : _algorithms) {
    _interrupt(algorithm);
  }
}

rbs_result_t CartesianRBSearch::find_rb(lit_vec_t const& base_assumption) {
  auto cartesian_set = IPS_TRACE_V(_pre_solve(base_assumption));
  if (_sbs_found) {
    return RBS_SBS_FOUND;
  } else if (IPS_UNLIKELY(_is_interrupted())) {
    return RBS_INTERRUPTED;
  } else {
    return _prepare_cartesian(std::move(cartesian_set));
  }
}

void CartesianRBSearch::_raise_for_sbs(int id) {
  bool expect = false;
  if (_sbs_found.compare_exchange_strong(expect, true)) {
    IPS_INFO("[" << id << "] Found SBS.");
    core::event::raise(event::SBS_FOUND);
  }
}

std::vector<std::vector<std::vector<Mini::Lit>>> CartesianRBSearch::_pre_solve(
    lit_vec_t const& base_assumption) {
  std::vector<std::vector<std::vector<Mini::Lit>>> non_conflict_assignments(
      _cfg.num_algorithms());
  IPS_VERIFY(
      _cfg.num_algorithms() > 0 && bool("num_algorithms must be positive"));
  std::stringstream algorithms_info;
  std::vector<std::thread> rb_search_threads;

  {
    std::lock_guard<std::mutex> lg(_m);
    for (uint32_t i = 0; i < _cfg.num_algorithms(); ++i) {
      _algorithms.push_back(
          ea::algorithm::RAlgorithm(ea::algorithm::AlgorithmRegistry::resolve(
              _cfg.algorithm_configs((int) i % _cfg.algorithm_configs_size()),
              _prop)));
    }
  }

  core::event::EventCallbackHandle sbs_found_handle = core::event::attach(
      [&] {
        IPS_INFO("Interrupting because SBS has been found.");
        interrupt();
        sbs_found_handle->detach();
      },
      event::SBS_FOUND);

  for (uint32_t i = 0; i < _cfg.num_algorithms() && !_interrupted; ++i) {
    uint32_t seed = util::random::sample<uint32_t>(0, UINT32_MAX);
    rb_search_threads.emplace_back([&, i, seed,
                                    config = _cfg.algorithm_configs(
                                        i % _cfg.algorithm_configs_size())] {
      util::random::Generator generator(seed);
      auto& algorithm = _algorithms[i];
      IPS_TRACE(algorithm->prepare(_preprocess));
      algorithm->set_base_assumption(base_assumption);
      IPS_TRACE(algorithm->process());
      if (_is_interrupted() || _sbs_found) {
        return;
      }
      auto const& rho_backdoor = algorithm->get_best();
      if (rho_backdoor.is_sbs()) {
        _raise_for_sbs(i);
        return;
      }

      std::atomic_uint32_t conflicts{0}, total{0};
      std::mutex nca_mutex;
      algorithm->get_prop().prop_search(
          search::createFullSearch(
              _preprocess->var_view(), rho_backdoor.get_vars().get_mask()),
          [&](bool conflict, auto const& assumption) {
            ++total;
            if (!conflict) {
              std::lock_guard<std::mutex> lg(nca_mutex);
              non_conflict_assignments[i].push_back(to_std_assump(assumption));
            } else {
              ++conflicts;
            }
            return !_sbs_found && !_is_interrupted();
          });

      if (non_conflict_assignments[i].empty()) {
        _raise_for_sbs(i);
      }

      {
        std::lock_guard<std::mutex> lg(_m);
        algorithms_info << "\n[Thread " << i << "]\n"
                        << "\tNumber of points visited: "
                        << algorithm->inaccurate_points() << '\n'
                        << "\tThe best backdoor is: " << rho_backdoor << '\n'
                        << "\tConflicts: " << conflicts << ", total: " << total
                        << "\n\tActual rho value: "
                        << (double) conflicts / (double) total << '\n';
        if (_sbs_found) {
          algorithms_info << "\tMay be stopped because SBS has been found.\n";
        }
        _algorithms[i].reset();
      }
    });
  }

  for (auto& thread : rb_search_threads) {
    IPS_VERIFY(thread.joinable());
    thread.join();
  }
  {
    std::lock_guard<std::mutex> lg(_m);
    _algorithms.clear();
  }
  IPS_INFO("Algorithms processing info: " << algorithms_info.str());
  return non_conflict_assignments;
}

search::USearch CartesianRBSearch::_prepare_cartesian(
    std::vector<std::vector<std::vector<Mini::Lit>>>&& cartesian_set) {
  std::vector<std::vector<std::vector<Mini::Lit>>> result;
  uint32_t max_non_conflict = _cfg.max_unpropagated();
  uint32_t max_cartesian_size = _cfg.max_cartesian_size();

  auto it = std::min_element(
      cartesian_set.begin(), cartesian_set.end(),
      [](auto const& a, auto const& b) { return a.size() < b.size(); });

  if (it->size() > max_non_conflict) {
    IPS_INFO("Sets are too large. Taking minimal with size " << it->size());
    result.push_back(std::move(*it));
  } else {
    uint64_t size = 1;
    for (auto& c_set : cartesian_set) {
      uint32_t cur_size = c_set.size();
      if (cur_size > max_non_conflict) {
        IPS_INFO("Skip because set is too large: " << cur_size);
        continue;
      }
      if (cur_size * size > max_cartesian_size) {
        IPS_INFO("Skip because result is too large: " << cur_size);
        continue;
      }
      IPS_INFO("Take set with size " << cur_size);
      result.push_back(std::move(c_set));
      size *= cur_size;
    }
  }

  IPS_INFO("Creating cartesian search");
  return search::createCartesianSearch(std::move(result));
}

REGISTER_PROTO(RBSearch, CartesianRBSearch, cartesian_rb_search_config);

}  // namespace core::solve
