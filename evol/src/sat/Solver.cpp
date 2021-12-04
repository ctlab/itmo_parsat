#include "evol/include/sat/Solver.h"

#include <iostream>

namespace ea::sat {

bool Solver::propagate(Minisat::vec<Minisat::Lit> const& assumptions) {
  Minisat::vec<Minisat::Lit> props;
  return propagate(assumptions, props);
}

State Solver::solve_limited() {
  Minisat::vec<Minisat::Lit> assumptions;
  return solve_limited(assumptions);
}

void Solver::solve_assignments(
    domain::UAssignment assignment_p, slv_callback_t const& callback) {
  domain::Assignment& assignment = *assignment_p;
  do {
    Minisat::vec<Minisat::Lit> const& assumptions = assignment();
    if (!callback(solve_limited(assumptions), assumptions)) {
      break;
    }
  } while (++assignment);
}

void Solver::prop_assignments(
    domain::UAssignment assignment_p, prop_callback_t const& callback) {
  domain::Assignment& assignment = *assignment_p;
  Minisat::vec<Minisat::Lit> propagated;
  do {
    Minisat::vec<Minisat::Lit> const& assumptions = assignment();
    if (!callback(propagate(assumptions, propagated), assumptions, propagated)) {
      break;
    }
  } while (++assignment);
}

}  // namespace ea::sat