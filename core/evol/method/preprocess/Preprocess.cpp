#include "core/evol/method/preprocess/Preprocess.h"

#include <utility>
#include <future>

namespace {

void collect_stats(
    core::sat::MinisatSimpBase& prop, core::lit_vec_t const& assumptions,
    core::lit_vec_t& propagated, std::set<int>& collection) {
  (void) prop.prop_check(assumptions, propagated);
  for (int j = 0; j < propagated.size(); ++j) { collection.insert(var(propagated[(int) j])); }
}

}  // namespace

namespace ea::preprocess {

Preprocess::Preprocess(PreprocessConfig config, core::sat::prop::RProp prop)
    : _prop(std::move(prop)), _config(std::move(config)) {}

bool Preprocess::preprocess(core::sat::Problem const& problem) {
  _num_vars = _prop->num_vars();
  if (_num_vars == 0) { return false; }
  auto workers = _prop->native();
  //  std::vector<core::sat::MinisatSimpBase> workers(16);

  size_t concurrency = workers.size();
  uint32_t vars_per_thread = _num_vars / concurrency;
  uint32_t add_vars_per_thread = _num_vars % concurrency;

  std::mutex result_mutex;
  std::vector<std::thread> worker_threads;
  std::vector<std::pair<int, int>> stats;
  worker_threads.reserve(concurrency);
  stats.reserve(_num_vars);

  for (int i = 0; i < workers.size(); ++i) {
    IPS_VERIFY(workers[i]->num_vars() == workers[0]->num_vars());
  }

  for (uint32_t i = 0, start_var = 0; i < concurrency && start_var < _num_vars; ++i) {
    uint32_t cur_num_vars = vars_per_thread + (i < add_vars_per_thread);
    uint32_t end_var = std::min(_num_vars, start_var + cur_num_vars);
    worker_threads.emplace_back([&problem, start_var, end_var, &workers, i, &result_mutex, &stats] {
      auto* worker = workers[i];
      //      worker.load_problem(problem);
      core::lit_vec_t assumptions(1), propagated;
      std::vector<std::pair<int, int>> stats_thread;
      for (uint32_t var = start_var; var < end_var; ++var) {
        std::set<int> prop_both;
        assumptions[0] = Mini::mkLit((int) var, true);
        collect_stats(*worker, assumptions, propagated, prop_both);
        assumptions[0] = Mini::mkLit((int) var, false);
        collect_stats(*worker, assumptions, propagated, prop_both);
        stats_thread.emplace_back(prop_both.size(), var);
      }
      std::lock_guard<std::mutex> lg(result_mutex);
      stats.insert(stats.end(), stats_thread.begin(), stats_thread.end());
    });
    start_var = end_var;
  }
  for (auto& thread : worker_threads) {
    IPS_VERIFY(thread.joinable());
    thread.join();
  }

  uint32_t max_watched_count = std::min(_config.heuristic_size(), _num_vars);
  std::sort(stats.begin(), stats.end());
  auto it = stats.crbegin();

  /// @brief Check if the 'best' variable has propagated more than one literal.
  /// otherwise, it makes no sense to use EA for this problem.
  if (it->first <= 1) { return false; }

  for (uint32_t i = 0; i < max_watched_count && it->first > 1; ++i, ++it) {
    _var_view.map_var((int) i, it->second);
  }

  if (core::Logger::should_log(LogType::HEURISTIC_RESULT)) {
    std::stringstream ss;
    for (auto iter = stats.crbegin(); iter != stats.crbegin() + (int) max_watched_count; ++iter) {
      ss << "{ prop: " << iter->first << ", var: " << iter->second << " }\n";
    }
    IPS_INFO("Heuristic init:\n" << ss.str());
  }
  return true;
}

core::domain::VarView const& Preprocess::var_view() const noexcept {
  return _var_view;
}

uint32_t Preprocess::num_vars() const noexcept {
  return _num_vars;
}

}  // namespace ea::preprocess