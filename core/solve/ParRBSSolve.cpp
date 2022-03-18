#include "core/solve/ParRBSSolve.h"

#include <utility>

namespace {

std::vector<Mini::Lit> to_std_assump(Mini::vec<Mini::Lit> const& assumption) {
  std::vector<Mini::Lit> result(assumption.size());
  for (int i = 0; i < assumption.size(); ++i) {
    result[i] = assumption[i];
  }
  return result;
}

}  // namespace

namespace core::solve {

ParRBSSolve::ParRBSSolve(ParRBSSolveConfig config)
    : RBSSolveBase(
          config.preprocess_config(), config.prop_config(),
          config.solver_service_config())
    , _cfg(std::move(config)) {}

void ParRBSSolve::_raise_for_sbs(uint32_t algorithm_id) noexcept {
  bool expect = false;
  if (_sbs_found.compare_exchange_strong(expect, true)) {
    IPS_INFO("[" << algorithm_id << "] Found SBS.");
    core::event::raise(event::SBS_FOUND);
  }
}

std::vector<std::vector<std::vector<Mini::Lit>>> ParRBSSolve::_pre_solve(
    sat::Problem const& problem,
    ea::preprocess::RPreprocess const& preprocess) {
  std::vector<std::vector<std::vector<Mini::Lit>>> non_conflict_assignments(
      _cfg.num_algorithms());
  IPS_VERIFY(
      _cfg.num_algorithms() > 0 && bool("num_algorithms must be positive"));
  std::stringstream algorithms_info;
  std::vector<ea::algorithm::RAlgorithm> algorithms;
  std::vector<std::thread> rbs_search_threads;
  std::mutex mutex;

  for (uint32_t i = 0; i < _cfg.num_algorithms(); ++i) {
    algorithms.push_back(
        ea::algorithm::RAlgorithm(ea::algorithm::AlgorithmRegistry::resolve(
            _cfg.algorithm_configs(i % _cfg.algorithm_configs_size()), _prop)));
  }

  _do_interrupt = [&] {
    std::lock_guard<std::mutex> lg(mutex);
    for (auto& algorithm : algorithms) {
      if (algorithm) {
        algorithm->interrupt();
      }
    }
  };

  core::event::EventCallbackHandle sbs_found_handle = core::event::attach(
      [&] {
        IPS_INFO("Interrupting because SBS has been found.");
        _do_interrupt();
        sbs_found_handle->detach();
      },
      event::SBS_FOUND);

  for (uint32_t i = 0; i < _cfg.num_algorithms(); ++i) {
    uint32_t seed = util::random::sample<uint32_t>(0, UINT32_MAX);
    rbs_search_threads.emplace_back([&, i, seed,
                                     config = _cfg.algorithm_configs(
                                         i % _cfg.algorithm_configs_size())] {
      util::random::Generator generator(seed);
      auto& algorithm = algorithms[i];
      algorithm->prepare(preprocess);
      //          auto& alg_prop = algorithm->get_prop();
      //          IPS_TRACE(alg_prop.load_problem(problem));
      IPS_TRACE(algorithm->process());

      auto const& rho_backdoor = algorithm->get_best();
      if (_is_interrupted() || _sbs_found) {
        return;
      }
      if (rho_backdoor.is_sbs()) {
        _raise_for_sbs(i);
        return;
      }

      std::atomic_uint32_t conflicts{0}, total{0};
      std::mutex nca_mutex;
      algorithm->get_prop().prop_assignments(
          domain::createFullSearch(
              preprocess->var_view(), rho_backdoor.get_vars().get_mask()),
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
        std::lock_guard<std::mutex> lg(mutex);
        algorithms_info << "\n[Thread " << i << "]\n";
        algorithms_info << "\tNumber of points visited: "
                        << algorithm->inaccurate_points() << '\n';
        algorithms_info << "\tThe best backdoor is: " << rho_backdoor << '\n';
        algorithms_info << "\tConflicts: " << conflicts << ", total: " << total
                        << '\n';
        algorithms_info << "\tActual rho value: "
                        << (double) conflicts / (double) total << '\n';
        if (_sbs_found) {
          algorithms_info << "\tMay be stopped because SBS has been found.\n";
        }

        algorithms[i].reset();
      }
    });
  }

  for (auto& thread : rbs_search_threads) {
    IPS_VERIFY(thread.joinable());
    thread.join();
  }
  _do_interrupt = {};
  IPS_INFO("Algorithms processing info: " << algorithms_info.str());
  return non_conflict_assignments;
}

domain::USearch ParRBSSolve::_prepare_cartesian(
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
  return domain::createCartesianSearch(std::move(result));
}

sat::State ParRBSSolve::_solve_impl(
    sat::Problem const& problem,
    ea::preprocess::RPreprocess const& preprocess) {
  auto cartesian_set = IPS_TRACE_V(_pre_solve(problem, preprocess));
  if (_sbs_found) {
    IPS_INFO("SBS has been found during propagation, thus result is UNSAT");
    return sat::UNSAT;
  }
  if (_is_interrupted()) {
    return sat::UNKNOWN;
  }
  return _solve_final(
      _filter_conflict(_prepare_cartesian(std::move(cartesian_set))));
}

REGISTER_PROTO(Solve, ParRBSSolve, par_rbs_solve_config);

}  // namespace core::solve