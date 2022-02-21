#include "core/solve/Solve.h"

namespace core {

void Solve::interrupt() {
  _interrupted = true;
  if (_do_interrupt) {
    _do_interrupt();
  }
}

bool Solve::_is_interrupted() const noexcept {
  return _interrupted;
}

sat::solver::RSolver Solve::_resolve_solver(SolverConfig const& config) {
  return sat::solver::RSolver(sat::solver::SolverRegistry::resolve(config));
}

sat::State Solve::_final_solve(sat::solver::Solver& solver, domain::USearch assignment) {
  IPS_VERIFY(!assignment->empty() && bool("Trying to final-solve empty assignment set"));
  std::mutex progress_lock;
  std::atomic_bool unknown{false}, satisfied{false};
  std::atomic_uint64_t conflicts{0}, total{0};
  boost::timer::progress_display progress(assignment->size(), std::cerr);
  IPS_TRACE_N(
      "Solver::final_solve",
      solver.solve_assignments(
          std::move(assignment), [&](sat::State result, bool conflict, auto&&) {
            if (_interrupted) {
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
  IPS_INFO("Unknown: " << unknown << ", interrupted: " << _interrupted);
  if (satisfied) {
    return core::sat::SAT;
  } else if (!unknown && !_interrupted) {
    return core::sat::UNSAT;
  } else {
    return core::sat::UNKNOWN;
  }
}

}  // namespace core