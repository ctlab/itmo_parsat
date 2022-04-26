#ifndef ITMO_PARSAT_REDUCESOLVE_H
#define ITMO_PARSAT_REDUCESOLVE_H

#include <list>
#include <boost/timer/progress_display.hpp>

#include "core/evol/method/preprocess/Preprocess.h"
#include "core/sat/solver/service/SolverService.h"
#include "core/solve/Solve.h"
#include "core/sat/Problem.h"
#include "util/WorkerPool.h"
#include "util/Reference.h"
#include "util/Registry.h"

namespace core::solve {

/**
 * @brief The base class for all algorithms that use rho-backdoor search
 * reduction.
 */
class ReduceSolve : public Solve {
 public:
  ReduceSolve(
      PreprocessConfig const& preprocess_config, PropConfig const& prop_config,
      SolverServiceConfig const& solver_service_config);

  [[nodiscard]] std::vector<lit_vec_t> _filter_conflict(search::USearch search);

  [[nodiscard]] sat::State solve(sat::Problem const& problem) final;

  sat::sharing::SharingUnit sharing_unit() noexcept override;

  Mini::vec<Mini::lbool> get_model() const override;

 protected:
  [[nodiscard]] sat::State _solve_final(std::vector<lit_vec_t> const& assumptions_v);

  [[nodiscard]] virtual sat::State _solve_impl(ea::preprocess::RPreprocess const& preprocess) = 0;

  void _interrupt_impl() override;

  sat::prop::RProp _prop;
  ea::preprocess::RPreprocess _preprocess;
  sat::solver::RSolverService _solver_service;

  std::mutex _model_lock;
  Mini::vec<Mini::lbool> _model;

 private:
  void _load_problem(sat::Problem const& problem);
};

MAKE_REFS(ReduceSolve);

}  // namespace core::solve

#endif  // ITMO_PARSAT_REDUCESOLVE_H
