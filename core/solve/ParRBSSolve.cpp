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

std::vector<std::vector<std::vector<Minisat::Lit>>> ParRBSSolve::_pre_solve(
    std::filesystem::path const& input) {
  std::vector<std::vector<std::vector<Minisat::Lit>>> non_conflict_assignments(
      _cfg.num_algorithms());
  bool ignore_algorithm_seeds =
      _cfg.num_algorithms() != static_cast<uint32_t>(_cfg.algorithm_configs_size());
  IPS_WARNING_IF(
      ignore_algorithm_seeds,
      "Algorithms' random seeds will be ignored. Instead, root generator will provide seed");
  IPS_VERIFY(_cfg.num_algorithms() > 0 && bool("num_algorithms must be positive"));

  std::vector<std::thread> rbs_search_threads;
  for (uint32_t i = 0; i < _cfg.num_algorithms(); ++i) {
    uint32_t seed = ignore_algorithm_seeds ? core::random::sample<uint32_t>(0, UINT32_MAX)
                                           : _cfg.algorithm_configs(i).random_seed();
    rbs_search_threads.emplace_back(
        [&, i, seed, config = _cfg.algorithm_configs(i % _cfg.algorithm_configs_size())] {
          core::Generator generator(seed);
          ea::algorithm::RAlgorithm algorithm(ea::algorithm::AlgorithmRegistry::resolve(config));
          auto& algorithm_solver = algorithm->get_solver();
          IPS_TRACE(algorithm_solver.parse_cnf(input));
          algorithm->prepare();

          SigHandler::handle_t alg_int_handle = core::sig::register_callback([&](int) {
            algorithm->interrupt();
            IPS_INFO("[" << i << "] Algorithm has been interrupted.");
            alg_int_handle->remove();
          });
          IPS_TRACE(algorithm->process());
          auto& rho_backdoor = algorithm->get_best();
          IPS_INFO("[" << i << "] Number of points visited: " << algorithm->inaccurate_points());
          IPS_INFO("[" << i << "] The best backdoor is: " << rho_backdoor);

          // Try to propagate all assumptions and collect ones that ended with no conflict
          std::mutex i_mutex;
          std::atomic_uint32_t conflicts{0}, total{0};
          algorithm->get_solver().prop_assignments(
              domain::createFullSearch(
                  algorithm->get_shared_data().var_view, rho_backdoor.get_vars().get_mask()),
              [&](bool conflict, auto const& assumption, auto&&) {
                ++total;
                if (!conflict) {
                  auto std_assumption = to_std_assump(assumption);
                  std::lock_guard<std::mutex> lg(i_mutex);
                  non_conflict_assignments[i].push_back(std_assumption);
                } else {
                  ++conflicts;
                }
                return true;
              });

          IPS_INFO("[" << i << "] Conflicts: " << conflicts << ", total: " << total);
          IPS_INFO("[" << i << "] Actual rho value: " << (double) conflicts / (double) total);
          if (non_conflict_assignments[i].empty()) {
            // This rho-backdoor is actually SBS, thus no need to solve further.
            std::raise(SIGINT);
          }
        });
  }

  // Wait for all algorithms to complete
  for (auto& thread : rbs_search_threads) {
    IPS_VERIFY(thread.joinable());
    thread.join();
  }

  return non_conflict_assignments;
}

std::vector<Minisat::vec<Minisat::Lit>> ParRBSSolve::_build_cartesian_product(
    std::vector<std::vector<std::vector<Minisat::Lit>>>&& non_conflict_assignments) {
  std::vector<size_t> non_conflict_sizes;
  for (auto const& i_non_conflict_assignments : non_conflict_assignments) {
    non_conflict_sizes.push_back(i_non_conflict_assignments.size());
    if (i_non_conflict_assignments.empty()) {
      return {};
    }
  }

  IPS_INFO("Non-conflict set sizes: " << non_conflict_sizes);
  IPS_INFO("Only sets with sizes less or equal to " << _cfg.max_unpropagated() << " will be used.");

  // Build cartesian product of non-conflict assumptions.
  uint32_t max_non_conflict = _cfg.max_unpropagated();
  size_t index = 0;
  while (index < non_conflict_assignments.size() &&
         non_conflict_assignments[index].size() > max_non_conflict) {
    ++index;
  }

  std::vector<std::vector<Minisat::Lit>> std_cartesian;
  size_t cartesian_expected_size;

  if (index == non_conflict_assignments.size()) {
    // All rho-backdoors have a lot of non-conflict assignments, so
    // we roll back to RBS algorithm: take the one with the least amount of
    // non-conflict assignments.
    auto it = std::min_element(
        non_conflict_assignments.begin(), non_conflict_assignments.end(),
        [](auto const& a, auto const& b) { return a.size() < b.size(); });
    std_cartesian = std::move(*it);
  } else {
    std_cartesian = std::move(non_conflict_assignments[index++]);
    cartesian_expected_size = std_cartesian.size();

    for (; index < non_conflict_assignments.size(); ++index) {
      if (non_conflict_assignments[index].size() > max_non_conflict) {
        continue;
      }
      cartesian_expected_size *= non_conflict_assignments[index].size();
      size_t old_assumptions_size = std_cartesian.size();
      for (size_t j = 0; j < old_assumptions_size; ++j) {
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
  core::Generator generator(_cfg.random_seed());

  // Build cartesian product of non-conflict assignments
  auto cartesian_product = _build_cartesian_product(_pre_solve(input));
  if (cartesian_product.empty()) {
    IPS_INFO("SBS has been found during propagation, thus result is UNSAT");
    return sat::UNSAT;
  }

  // Solve for the built assignments
  auto solver = _resolve_solver(_cfg.solver_config());
  IPS_TRACE(solver->parse_cnf(input));
  return _final_solve(*solver, domain::createCustomSearch(cartesian_product));
}

REGISTER_PROTO(Solve, ParRBSSolve, par_rbs_solve_config);

}  // namespace core