#include "evol/include/sat/Solver.h"

namespace ea::sat {

bool Solver::propagate(Minisat::vec<Minisat::Lit> const& assumps) {
  Minisat::vec<Minisat::Lit> props;
  return propagate(assumps, props);
}

void Solver::solve_limited() {
  Minisat::vec<Minisat::Lit> assumptions;
  solve_limited(assumptions);
}

}  // namespace ea::sat