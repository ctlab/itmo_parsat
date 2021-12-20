#include "core/solve/Solve.h"

namespace core {

sat::RSolver Solve::_resolve_solver(SolverConfig const& config) {
  return sat::RSolver(sat::SolverRegistry::resolve(config));
}

sat::State Solve::_final_solve(sat::Solver& solver, domain::UAssignment assignment) {
  IPS_VERIFY(!assignment->empty() && bool("Trying to final-solve empty assignment set"));

  std::atomic_bool interrupted{false};
  core::sig::unset();
  SigHandler::handle_t slv_int_handle = core::sig::register_callback([&](int) {
    solver.interrupt();
    interrupted = true;
    IPS_INFO("Solver has been interrupted.");
    slv_int_handle->remove();
  });

  std::mutex progress_lock;
  std::atomic_bool unknown{false}, satisfied{false};
  std::atomic_uint64_t conflicts{0}, total{0};
  boost::timer::progress_display progress(assignment->size(), std::cerr);

  IPS_TRACE_N(
      "Solver::final_solve",
      solver.solve_assignments(
          std::move(assignment), [&](sat::State result, bool conflict, auto&&) {
            if (core::sig::is_set()) {
              return false;
            }
            ++total;
            conflicts += conflict;
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

  IPS_INFO("Conflicts: " << conflicts << ", total: " << total);
  IPS_INFO("Conflict rate is: " << (double) conflicts / (double) total);

  core::sig::unset();
  if (satisfied) {
    return core::sat::SAT;
  } else if (!unknown && !interrupted) {
    return core::sat::UNSAT;
  } else {
    return core::sat::UNKNOWN;
  }
}

}  // namespace core