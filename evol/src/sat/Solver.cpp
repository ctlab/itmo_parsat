#include "evol/include/sat/Solver.h"

namespace ea::sat {

bool Solver::propagate(Minisat::vec<Minisat::Lit> const& assumptions) {
  Minisat::vec<Minisat::Lit> props;
  return propagate(assumptions, props);
}

void Solver::solve_limited() {
  Minisat::vec<Minisat::Lit> assumptions;
  solve_limited(assumptions);
}

}  // namespace ea::sat