#include "core/sat/Solver.h"

namespace core::sat {

bool Solver::propagate(Minisat::vec<Minisat::Lit> const& assumptions) {
  clear_interrupt();
  Minisat::vec<Minisat::Lit> props;
  return propagate(assumptions, props);
}

State Solver::solve_limited() {
  clear_interrupt();
  Minisat::vec<Minisat::Lit> assumptions;
  return solve_limited(assumptions);
}

void Solver::solve_assignments(domain::USearch assignment_p, slv_callback_t const& callback) {
  clear_interrupt();
  domain::Search& assignment = *assignment_p;
  START_ASGN_TRACK(assignment_p->size());
  do {
    Minisat::vec<Minisat::Lit> const& assumptions = assignment();
    ASGN_TRACK(assumptions);
    bool conflict = propagate(assumptions);
    State result = conflict ? UNSAT : solve_limited(assumptions);
    if (!callback(result, conflict, assumptions)) {
      BREAK_ASGN_TRACK;
      break;
    }
  } while (!IPS_UNLIKELY(interrupted() || !(++assignment)));
  END_ASGN_TRACK;
}

void Solver::prop_assignments(core::domain::USearch assignment_p, prop_callback_t const& callback) {
  clear_interrupt();
  domain::Search& assignment = *assignment_p;
  Minisat::vec<Minisat::Lit> propagated;
  START_ASGN_TRACK(assignment_p->size());
  do {
    Minisat::vec<Minisat::Lit> const& assumptions = assignment();
    ASGN_TRACK(assumptions);
    if (!callback(propagate(assumptions, propagated), assumptions, propagated)) {
      BREAK_ASGN_TRACK;
      break;
    }
  } while (!IPS_UNLIKELY(interrupted() || !(++assignment)));
  END_ASGN_TRACK;
}

void Solver::interrupt() {
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

}  // namespace core::sat