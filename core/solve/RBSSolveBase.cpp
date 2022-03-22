#include "RBSSolveBase.h"

#include "util/stream.h"

namespace core::solve {

RBSSolveBase::RBSSolveBase(
    PreprocessConfig const& preprocess_config, PropConfig const& prop_config,
    SolverServiceConfig const& solver_service_config)
    : _preprocess(new ea::preprocess::Preprocess(preprocess_config))
    , _prop(core::sat::prop::PropRegistry::resolve(prop_config))
    , _solver_service(core::sat::solver::SolverServiceRegistry::resolve(
          solver_service_config)) {}

std::vector<Mini::vec<Mini::Lit>> RBSSolveBase::_filter_conflict(
    domain::USearch assignment) {
  IPS_INFO("Filtering conflict assignments");
  std::mutex progress_lock;
  std::atomic_uint64_t conflicts{0}, total{0};
  boost::timer::progress_display progress(assignment->size(), std::cerr);

  std::vector<Mini::vec<Mini::Lit>> non_conflict;
  std::mutex nc_lock;

  IPS_TRACE_N("RBSSolveBase::filter_conflict", {
    _prop->prop_assignments(
        std::move(assignment),
        [&](bool conflict, Mini::vec<Mini::Lit> const& assumptions) {
          if (_interrupted) {
            return false;
          }
          bool resume = true;
          if (!conflict) {
            std::lock_guard<std::mutex> lg(nc_lock);
            non_conflict.push_back(assumptions);
          }
          ++total;
          conflicts += conflict;
          {
            std::lock_guard<std::mutex> lg(progress_lock);
            ++progress;
          }
          return resume;
        });
  });
  IPS_INFO("Conflicts: " << conflicts << ", total: " << total);
  IPS_INFO("Conflict rate is: " << (double) conflicts / (double) total);
  return non_conflict;
}

sat::State RBSSolveBase::_solve_final(
    std::vector<Mini::vec<Mini::Lit>> const& assumptions_v) {
  IPS_INFO(
      "Solving non-conflict assignments (" << assumptions_v.size() << ").");
  std::atomic_bool unknown{false};
  boost::timer::progress_display progress(assumptions_v.size(), std::cerr);
  _do_interrupt = [this] { _solver_service->interrupt(); };

  IPS_TRACE_N("RBSSolveBase::solve_final", {
    std::vector<std::future<sat::State>> results;
    for (auto& assumption : assumptions_v) {
      results.push_back(_solver_service->solve(
          assumption, sat::solver::SolverService::DUR_INDEF));
    }
    for (auto& result : results) {
      if (_interrupted) {
        break;
      }
      switch (result.get()) {
        case core::sat::UNSAT:
          break;
        case core::sat::UNKNOWN:
          unknown = true;
          break;
        case core::sat::SAT:
          return core::sat::SAT;
      }
      ++progress;
    }
    if (!unknown && !_interrupted) {
      return core::sat::UNSAT;
    } else {
      return core::sat::UNKNOWN;
    }
  });
}

void RBSSolveBase::_load_problem(sat::Problem const& problem) {
  _prop->load_problem(problem);
  _solver_service->load_problem(problem);
}

sat::State RBSSolveBase::solve(sat::Problem const& problem) {
  IPS_TRACE_N("RBSSolveBase::load_problem", _load_problem(problem));
  if (!IPS_TRACE_N_V(
          "RBSSolveBase::preprocess", _preprocess->preprocess(problem))) {
    return _solver_service->solve({}, sat::solver::SolverService::DUR_INDEF)
        .get();
  } else {
    return _solve_impl(problem, _preprocess);
  }
}

}  // namespace core::solve
