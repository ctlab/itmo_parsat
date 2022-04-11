#include "core/evol/method/preprocess/Preprocess.h"

#include <utility>

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

namespace ea::preprocess {

Preprocess::Preprocess(PreprocessConfig config) : _config(std::move(config)) {}

bool Preprocess::preprocess(core::sat::Problem const& problem) {
  core::sat::prop::SimpProp prop;
  prop.load_problem(problem);

  Mini::vec<Mini::Lit> assumptions(1);
  Mini::vec<Mini::Lit> propagated;
  std::vector<std::pair<int, int>> stats;
  stats.reserve(prop.num_vars());

  _num_vars = prop.num_vars();
  if (prop.num_vars() == 0) {
    return false;
  }

  for (unsigned i = 0; i < prop.num_vars(); ++i) {
    std::set<int> prop_both;
    assumptions[0] = Mini::mkLit((int) i, true);
    collect_stats(prop, assumptions, propagated, prop_both);
    assumptions[0] = Mini::mkLit((int) i, false);
    collect_stats(prop, assumptions, propagated, prop_both);
    stats.emplace_back(prop_both.size(), i);
  }

  uint32_t max_watched_count =
      std::min(_config.heuristic_size(), prop.num_vars());
  std::sort(stats.begin(), stats.end());
  auto it = stats.crbegin();

  /// @brief Check if the 'best' variable has propagated more than one literal.
  /// otherwise, it makes no sense to use EA for this problem.
  if (it->first <= 1) {
    return false;
  }

  for (uint32_t i = 0; i < max_watched_count && it->first > 1; ++i, ++it) {
    _var_view.map_var((int) i, it->second);
  }

  if (core::Logger::should_log(LogType::HEURISTIC_RESULT)) {
    std::stringstream ss;
    for (auto iter = stats.crbegin();
         iter != stats.crbegin() + (int) max_watched_count; ++iter) {
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