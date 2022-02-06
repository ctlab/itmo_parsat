#include "core/solve/ParRBSSolve.h"

#include <utility>

namespace {

std::vector<Minisat::Lit> to_std_assump(Minisat::vec<Minisat::Lit> const& assumption) {
  std::vector<Minisat::Lit> result(assumption.size());
  for (int i = 0; i < assumption.size(); ++i) {
    result[i] = assumption[i];
  }
  return result;
}

Minisat::vec<Minisat::Lit> to_ms_assump(std::vector<Minisat::Lit> const& assumption) {
  Minisat::vec<Minisat::Lit> result((int) assumption.size());
  for (int i = 0; i < result.size(); ++i) {
    result[i] = assumption[i];
  }
  return result;
}

}  // namespace

namespace core {

ParRBSSolve::ParRBSSolve(ParRBSSolveConfig config) : _cfg(std::move(config)) {}

void ParRBSSolve::_raise_for_sbs(int algorithm_id) noexcept {
  bool expect = false;
  if (_sbs_found.compare_exchange_strong(expect, true)) {
    IPS_INFO("[" << algorithm_id << "] Found SBS.");
    core::event::raise(event::SBS_FOUND);
  }
}

std::vector<std::vector<std::vector<Minisat::Lit>>> ParRBSSolve::_pre_solve(
    std::filesystem::path const& input) {
  std::vector<std::vector<std::vector<Minisat::Lit>>> non_conflict_assignments(
      _cfg.num_algorithms());
  IPS_VERIFY(_cfg.num_algorithms() > 0 && bool("num_algorithms must be positive"));
  std::stringstream algorithms_info;
  std::vector<ea::algorithm::RAlgorithm> algorithms;
  std::vector<std::thread> rbs_search_threads;
  std::mutex mutex;

  for (uint32_t i = 0; i < _cfg.num_algorithms(); ++i) {
    algorithms.push_back(ea::algorithm::RAlgorithm(ea::algorithm::AlgorithmRegistry::resolve(
        _cfg.algorithm_configs(i % _cfg.algorithm_configs_size()))));
  }

  _do_interrupt = [&] {
    for (auto& algorithm : algorithms) {
      algorithm->interrupt();
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
    uint32_t seed = core::random::sample<uint32_t>(0, UINT32_MAX);
    rbs_search_threads.emplace_back(
        [&, i, seed, config = _cfg.algorithm_configs(i % _cfg.algorithm_configs_size())] {
          core::Generator generator(seed);
          auto& algorithm = algorithms[i];
          auto& algorithm_solver = algorithm->get_prop();
          IPS_TRACE(algorithm_solver.parse_cnf(input));
          algorithm->prepare();
          IPS_TRACE(algorithm->process());

          auto const& rho_backdoor = algorithm->get_best();
          if (_is_interrupted() || _sbs_found) {
            return;
          }
          if (rho_backdoor.is_sbs()) {
            _raise_for_sbs(i);
            return;
          }

          // Try to propagate all assumptions and collect ones that ended with no conflict
          std::atomic_uint32_t conflicts{0}, total{0};
          algorithm->get_prop().prop_assignments(
              domain::createFullSearch(
                  algorithm->get_shared_data().var_view, rho_backdoor.get_vars().get_mask()),
              [&](bool conflict, auto const& assumption) {
                ++total;
                if (!conflict) {
                  auto std_assumption = to_std_assump(assumption);
                  std::lock_guard<std::mutex> lg(mutex);
                  non_conflict_assignments[i].push_back(std_assumption);
                } else {
                  ++conflicts;
                }
                return !_sbs_found && !_is_interrupted();
              });

          if (non_conflict_assignments[i].empty()) {
            // This rho-backdoor is actually SBS, thus no need to solve further.
            _raise_for_sbs(i);
          }

          {  // add the best backdoor to log
            std::lock_guard<std::mutex> lg(mutex);
            algorithms_info << "[Thread " << i << "]\n";
            algorithms_info << "\tNumber of points visited: " << algorithm->inaccurate_points()
                            << '\n';
            algorithms_info << "\tThe best backdoor is: " << rho_backdoor << '\n';
            algorithms_info << "\tConflicts: " << conflicts << ", total: " << total << '\n';
            algorithms_info << "\tActual rho value: " << (double) conflicts / (double) total
                            << '\n';
            if (_sbs_found) {
              algorithms_info << "\tMay be stopped because SBS has been found.\n";
            }
          }
        });
  }

  // Wait for all algorithms to complete
  for (auto& thread : rbs_search_threads) {
    IPS_VERIFY(thread.joinable());
    thread.join();
  }
  _do_interrupt = {};
  IPS_INFO("Algorithms processing info: " << algorithms_info.str());
  return non_conflict_assignments;
}

std::vector<Minisat::vec<Minisat::Lit>> ParRBSSolve::_build_cartesian_product(
    std::vector<std::vector<std::vector<Minisat::Lit>>>&& non_conflict_assignments) {
  if (_sbs_found || _is_interrupted()) {
    return {};
  }
  std::vector<uint32_t> non_conflict_sizes;
  for (auto const& i_non_conflict_assignments : non_conflict_assignments) {
    non_conflict_sizes.push_back(i_non_conflict_assignments.size());
  }

  IPS_INFO("Non-conflict set sizes: " << non_conflict_sizes);
  IPS_INFO("Only sets with sizes less or equal to " << _cfg.max_unpropagated() << " will be used.");

  // Build cartesian product of non-conflict assumptions.
  uint32_t max_non_conflict = _cfg.max_unpropagated();
  uint32_t index = 0;
  while (index < non_conflict_assignments.size() &&
         non_conflict_assignments[index].size() > max_non_conflict) {
    ++index;
  }
  std::vector<std::vector<Minisat::Lit>> std_cartesian;
  uint32_t cartesian_expected_size;

  if (index == non_conflict_assignments.size()) {
    // All rho-backdoors have a lot of non-conflict assignments, so
    // we roll back to RBS algorithm: take the one with the least amount of
    // non-conflict assignments.
    auto it = std::min_element(
        non_conflict_assignments.begin(), non_conflict_assignments.end(),
        [](auto const& a, auto const& b) { return a.size() < b.size(); });
    std_cartesian = std::move(*it);
    cartesian_expected_size = std_cartesian.size();
  } else {
    std_cartesian = std::move(non_conflict_assignments[index++]);
    cartesian_expected_size = std_cartesian.size();

    for (; index < non_conflict_assignments.size(); ++index) {
      size_t cur_size = non_conflict_assignments[index].size();
      if (cur_size > max_non_conflict ||
          std_cartesian.size() * cur_size > _cfg.max_cartesian_size()) {
        continue;
      }
      cartesian_expected_size *= cur_size;
      uint32_t old_assumptions_size = std_cartesian.size();
      for (uint32_t j = 0; j < old_assumptions_size; ++j) {
        for (auto& i_non_conflict_assignments : non_conflict_assignments[index]) {
          auto j_assumption = std_cartesian[j];
          j_assumption.insert(
              j_assumption.end(), std::make_move_iterator(i_non_conflict_assignments.begin()),
              std::make_move_iterator(i_non_conflict_assignments.end()));
          std_cartesian.push_back(std::move(j_assumption));
        }
      }
      std_cartesian.erase(std_cartesian.begin(), std_cartesian.begin() + old_assumptions_size);
    }
  }

  IPS_INFO("Cartesian product size: " << std_cartesian.size());
  IPS_VERIFY(
      std_cartesian.size() == cartesian_expected_size &&
      bool("Internal error: failed to build cartesian product correctly"));

  // Convert assignments back to MS types
  std::vector<Minisat::vec<Minisat::Lit>> ms_cartesian;
  for (auto& assump : std_cartesian) {
    ms_cartesian.push_back(to_ms_assump(assump));
  }

  return ms_cartesian;
}

sat::State ParRBSSolve::solve(std::filesystem::path const& input) {
  // Build cartesian product of non-conflict assignments
  auto cartesian_product = IPS_TRACE_V(_build_cartesian_product(_pre_solve(input)));
  if (_sbs_found) {
    IPS_INFO("SBS has been found during propagation, thus result is UNSAT");
    return sat::UNSAT;
  }
  if (_is_interrupted()) {
    return sat::UNKNOWN;
  }
  // Solve for the built assignments
  auto solver = _resolve_solver(_cfg.solver_config());
  _do_interrupt = [solver] { solver->interrupt(); };
  IPS_TRACE(solver->parse_cnf(input));
  return _final_solve(*solver, domain::createCustomSearch(cartesian_product));
}

REGISTER_PROTO(Solve, ParRBSSolve, par_rbs_solve_config);

}  // namespace core