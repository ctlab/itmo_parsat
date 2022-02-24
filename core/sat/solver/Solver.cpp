#include "core/sat/solver/Solver.h"

namespace core::sat::solver {

State Solver::solve_limited() {
  clear_interrupt();
  vec_lit_t assumptions;
  return solve(assumptions);
}

void Solver::solve_assignments(domain::USearch assignment_p, slv_callback_t const& callback) {
  clear_interrupt();
  domain::Search& assignment = *assignment_p;
  START_ASGN_TRACK(assignment_p->size());
  do {
    vec_lit_t const& assumptions = assignment();
    ASGN_TRACK(assumptions);
    bool conflict = propagate_confl(assumptions);
    State result = conflict ? UNSAT : solve(assumptions);
    if (!callback(result, conflict, assumptions)) {
      BREAK_ASGN_TRACK;
      break;
    }
  } while (!IPS_UNLIKELY(interrupted() || !(++assignment)));
  END_ASGN_TRACK(interrupted());
}

void Solver::interrupt() {
  IPS_INFO_IF(!_interrupted, "Solver has been interrupted.");
  _interrupted = true;
  _do_interrupt();
}

void Solver::clear_interrupt() {
  _interrupted = false;
  _do_clear_interrupt();
}

bool Solver::interrupted() const {
  return _interrupted;
}

}  // namespace core::sat::solver