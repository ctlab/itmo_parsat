#include "core/solve/reduce/ReduceSolve.h"

namespace core::solve {

ReduceSolve::ReduceSolve(
    PreprocessConfig const& preprocess_config, PropConfig const& prop_config,
    SolverServiceConfig const& solver_service_config)
    : _prop(core::sat::prop::PropRegistry::resolve(prop_config))
    , _preprocess(new ea::preprocess::Preprocess(preprocess_config, _prop))
    , _solver_service(core::sat::solver::SolverServiceRegistry::resolve(solver_service_config)) {}

void ReduceSolve::_interrupt_impl() { _interrupt(_solver_service); }

std::vector<lit_vec_t> ReduceSolve::_filter_conflict(search::USearch assignment) {
  IPS_INFO("Filtering conflict assignments");
  std::mutex progress_lock, nc_lock;
  std::atomic_uint64_t conflicts{0}, total{0};
  boost::timer::progress_display progress(assignment->size(), std::cerr);
  std::vector<lit_vec_t> non_conflict;

  IPS_BLOCK(filter_conflict, {
    _prop->prop_search(std::move(assignment), [&](bool conflict, lit_vec_t const& assumption) {
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

sat::State ReduceSolve::_solve_final(std::vector<lit_vec_t> const& assumptions_v) {
  IPS_INFO("Solving non-conflict assignments (" << assumptions_v.size() << ").");
  std::atomic_bool unknown{false}, satisfied{false};
  std::mutex progress_lock;
  boost::timer::progress_display progress(assumptions_v.size(), std::cerr);

  IPS_BLOCK(reduce_solve_tasks, {
    std::vector<std::future<sat::State>> futures;
    futures.reserve(assumptions_v.size());
    for (auto& assumption : assumptions_v) {
      futures.push_back(_solver_service->solve(
          assumption, sat::solver::SolverService::DUR_INDEF,
          [this, &unknown, &satisfied, &progress, &progress_lock](sat::State result, auto model) {
            {
              std::lock_guard<std::mutex> lg(progress_lock);
              ++progress;
            }
            switch (result) {
              case core::sat::UNSAT: break;
              case core::sat::UNKNOWN: unknown = true; break;
              case core::sat::SAT:
                satisfied = true;
                IPS_VERIFY(model.has_value());
                {
                  std::lock_guard<std::mutex> lg(_model_lock);
                  _model = std::move(model.value());
                }
                _solver_service->interrupt();
                break;
            }
          }));
    }
    util::wait_for_futures(futures);

    if (satisfied) {
      return core::sat::SAT;
    } else if (!unknown && !_is_interrupted()) {
      return core::sat::UNSAT;
    } else {
      return core::sat::UNKNOWN;
    }
  });
}

void ReduceSolve::_load_problem(sat::Problem const& problem) {
  IPS_BLOCK(prop_load_problem, _prop->load_problem(problem));
  IPS_BLOCK(solver_service_load_problem, _solver_service->load_problem(problem));
}

sat::State ReduceSolve::solve(sat::Problem const& problem) {
  _load_problem(problem);
  if (!IPS_BLOCK_R(algorithm_preprocess, _preprocess->preprocess(problem))) {
    return _solver_service->solve({}, sat::solver::SolverService::DUR_INDEF).get();
  } else {
    return _solve_impl(_preprocess);
  }
}

sat::sharing::SharingUnit ReduceSolve::sharing_unit() noexcept { return _solver_service->sharing_unit(); }

Mini::vec<Mini::lbool> ReduceSolve::get_model() const { return _model; }

}  // namespace core::solve
