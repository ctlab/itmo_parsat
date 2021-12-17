#include "core/solve/SBSSolve.h"

namespace core {

SBSSolve::SBSSolve(SBSSolveConfig const& config) : _cfg(config) {}

sat::State SBSSolve::solve(std::filesystem::path const& input) {
  core::SigHandler sigh;
  auto solver = _resolve_solver(_cfg.solver_config());
  ea::algorithm::RAlgorithm algorithm(
      ea::algorithm::AlgorithmRegistry::resolve(_cfg.algorithm_config()));
  auto& algorithm_solver = algorithm->get_solver();

  IPS_TRACE(solver->parse_cnf(input));
  IPS_TRACE(algorithm_solver.parse_cnf(input));
  algorithm->prepare();

  SigHandler::handle_t alg_int_handle = sigh.register_callback([&alg_int_handle, &algorithm](int) {
    algorithm->interrupt();
    IPS_INFO("Algorithm has been interrupted.");
    alg_int_handle->remove();
  });

  IPS_TRACE(algorithm->process());
  auto& r_backdoor = algorithm->get_best();
  IPS_INFO("Number of points visited: " << algorithm->inaccurate_points());
  IPS_INFO("The best backdoor is: " << r_backdoor);

  std::atomic_bool satisfied = false, unknown = false;
  std::atomic_uint64_t propagated{0}, total{0};
  size_t num_vars = r_backdoor.fitness().pow_r;
  std::vector<bool> vars = r_backdoor.get_vars().get_mask();
  core::domain::UAssignment assignment_p =
      core::domain::createFullSearch(r_backdoor.var_view(), vars);
  std::mutex progress_lock;

  boost::timer::progress_display progress((unsigned long) std::pow(2UL, num_vars), std::cerr);

  bool interrupted = false;
  sigh.unset();
  SigHandler::handle_t slv_int_handle = sigh.register_callback([&](int) {
    solver->interrupt();
    interrupted = true;
    IPS_INFO("Solver has been interrupted.");
    slv_int_handle->remove();
  });

  // clang-format off
  IPS_TRACE_NAMED("solve_assignments", solver->solve_assignments(
      std::move(assignment_p), [&] (sat::State result, bool prop_conflict, auto) {
        if (sigh.is_set()) {
          return false;
        }
        if (prop_conflict) {
          ++propagated;
        }
        ++total;
        {
          std::lock_guard<std::mutex> lg(progress_lock);
          ++progress;
        }
        switch (result) {
          case core::sat::UNSAT:
            return true;
          case core::sat::UNKNOWN:
            unknown = true;
            return true;
          case core::sat::SAT:
            satisfied = true;
          default:
            return false;
        }
      }));
  // clang-format on

  sigh.unset();
  IPS_INFO("Prop: " << propagated << ", total: " << total);
  IPS_INFO("Actual rho is: " << (double) propagated / (double) total);

  if (satisfied) {
    return core::sat::SAT;
  } else if (!unknown && !interrupted) {
    return core::sat::UNSAT;
  } else {
    return core::sat::UNKNOWN;
  }
}

REGISTER_PROTO(Solve, SBSSolve, sbs_solve_config);

}  // namespace core