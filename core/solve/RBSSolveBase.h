#ifndef ITMO_PARSAT_RBSSOLVEBASE_H
#define ITMO_PARSAT_RBSSOLVEBASE_H

#include <variant>
#include <boost/timer/progress_display.hpp>

#include "core/sat/solver/sequential/Solver.h"
#include "core/sat/prop/Prop.h"
#include "util/EventHandler.h"
#include "util/SigHandler.h"
#include "util/TimeTracer.h"
#include "core/proto/solve_config.pb.h"
#include "core/solve/Solve.h"
#include "core/sat/solver/service/SequentialSolverService.h"
#include "core/evol/method/preprocess/Preprocess.h"

namespace core::solve {

/**
 * @brief Base for all classes using RBS strategy in one way or another
 */
class RBSSolveBase : public Solve {
 public:
  RBSSolveBase(
      PreprocessConfig const& preprocess_config, PropConfig const& prop_config,
      SolverServiceConfig const& solver_service_config);

  [[nodiscard]] std::vector<lit_vec_t> _filter_conflict(
      domain::USearch assignment);

  [[nodiscard]] sat::State solve(sat::Problem const& problem) final;

 protected:
  sat::State _solve_final(std::vector<lit_vec_t> const& assumptions_v);

  virtual sat::State _solve_impl(
      sat::Problem const& problem,
      ea::preprocess::RPreprocess const& preprocess) = 0;

 private:
  void _load_problem(sat::Problem const& problem);

 protected:
  ea::preprocess::RPreprocess _preprocess;
  core::sat::prop::RProp _prop;
  core::sat::solver::RSolverService _solver_service;
};

}  // namespace core::solve

#endif  // ITMO_PARSAT_RBSSOLVEBASE_H
