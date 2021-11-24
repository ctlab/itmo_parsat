#ifndef EVOL_SOLVER_H
#define EVOL_SOLVER_H

#include <minisat/core/SolverTypes.h>
#include <minisat/mtl/Vec.h>

#include <filesystem>
#include <memory>

namespace ea::sat {

enum State {
  SAT,
  UNSAT,
  UNKNOWN,
};

class Solver {
 public:
  virtual ~Solver() = default;

  /**
   * Parses cnf from .gz file with the specified path.
   */
  virtual void parse_cnf(std::filesystem::path const& path) = 0;

  /**
   * Returns the current state of the solver.
   */
  [[nodiscard]] virtual State state() const noexcept = 0;

  /**
   * Runs solveLimited implementation of SimpSolver.
   */
  void solve_limited();

  /**
   * Runs solveLimited implementation of SimpSolver with assumptions.
   */
  virtual void solve_limited(Minisat::vec<Minisat::Lit> const& assumptions) = 0;

  /**
   * Interrupt solver if the `solve*` is running.
   */
  virtual void interrupt() = 0;

  /**
   * Returns the number of variables in formula.
   */
  [[nodiscard]] virtual unsigned num_vars() const noexcept = 0;

  /**
   * Propagates a given list of assumptions.
   * Returns true iff conflicts happened.
   */
  virtual bool propagate(
      Minisat::vec<Minisat::Lit> const& assumptions, Minisat::vec<Minisat::Lit>& propagated) = 0;

  /**
   * Propagates a given list of assumptions.
   */
  [[nodiscard]] bool propagate(Minisat::vec<Minisat::Lit> const& assumptions);
};

using RSolver = std::shared_ptr<Solver>;

}  // namespace ea::sat

#endif  // EVOL_SOLVER_H