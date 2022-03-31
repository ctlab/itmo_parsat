#include "core/solve/reduce/ReduceSolve.h"

namespace core::solve {

ReduceSolve::ReduceSolve(
    PreprocessConfig const& preprocess_config, PropConfig const& prop_config,
    SolverServiceConfig const& solver_service_config)
    : _preprocess(new ea::preprocess::Preprocess(preprocess_config))
    , _prop(core::sat::prop::PropRegistry::resolve(prop_config))
    , _solver_service(core::sat::solver::SolverServiceRegistry::resolve(
          solver_service_config)) {}

void ReduceSolve::_interrupt_impl() {
  _interrupt(_solver_service);
}

std::vector<lit_vec_t> ReduceSolve::_filter_conflict(
    domain::USearch assignment) {
  IPS_INFO("Filtering conflict assignments");
  std::mutex progress_lock, nc_lock;
  std::atomic_uint64_t conflicts{0}, total{0};
  boost::timer::progress_display progress(assignment->size(), std::cerr);
  std::vector<lit_vec_t> non_conflict;

  IPS_TRACE_N("RBSSolveBase::filter_conflict", {
    _prop->prop_assignments(
        std::move(assignment), [&](bool conflict, lit_vec_t const& assumption) {
          if (_is_interrupted()) {
            return false;
          }
          bool resume = true;
          if (!conflict) {
            std::lock_guard<std::mutex> lg(nc_lock);
            non_conflict.push_back(assumption);
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

sat::State ReduceSolve::_solve_final(
    std::vector<lit_vec_t> const& assumptions_v) {
  IPS_INFO(
      "Solving non-conflict assignments (" << assumptions_v.size() << ").");
  std::atomic_bool unknown{false};
  boost::timer::progress_display progress(assumptions_v.size(), std::cerr);

  IPS_TRACE_N("RBSSolveBase::solve_final", {
    std::vector<std::future<sat::State>> results;
    results.reserve(assumptions_v.size());
    for (auto& assumption : assumptions_v) {
      results.push_back(_solver_service->solve(
          assumption, sat::solver::SolverService::DUR_INDEF));
    }
    for (auto& result : results) {
      if (_is_interrupted()) {
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
    if (!unknown && !_is_interrupted()) {
      return core::sat::UNSAT;
    } else {
      return core::sat::UNKNOWN;
    }
  });
}

void ReduceSolve::_load_problem(sat::Problem const& problem) {
  _prop->load_problem(problem);
  _solver_service->load_problem(problem);
}

sat::State ReduceSolve::solve(sat::Problem const& problem) {
  IPS_TRACE_N("ReduceSolve::load_problem", _load_problem(problem));
  if (!IPS_TRACE_N_V(
          "ReduceSolve::preprocess", _preprocess->preprocess(problem))) {
    return _solver_service->solve({}, sat::solver::SolverService::DUR_INDEF)
        .get();
  } else {
    return _solve_impl(_preprocess);
  }
}

sat::sharing::SharingUnit ReduceSolve::sharing_unit() noexcept {
  return _solver_service->sharing_unit();
}

}  // namespace core::solve
