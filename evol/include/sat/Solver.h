#ifndef EVOL_SOLVER_H
#define EVOL_SOLVER_H

#include <filesystem>

#include "evol/proto/config.pb.h"
#include "minisat/simp/SimpSolver.h"

namespace ea::sat {

enum State {
  SAT,
  UNSAT,
  UNKNOWN,
};

class Solver {
 public:
  explicit Solver(FullMinisatSolverConfig const& config);

  /**
   * Parses cnf from .gz file with the specified path.
   */
  void parse_cnf(std::filesystem::path const& path);

  /**
   * Returns the current state of the solver.
   */
  [[nodiscard]] State state() const noexcept;

  /**
   * Runs solveLimited implementation of SimpSolver.
   */
  void solve_limited();

  /**
   * Interrupt solver if the `solve*` is running.
   */
  void interrupt();

 private:
  Minisat::SimpSolver impl_;
  State state_ = UNKNOWN;
  bool preprocess_;
};

}  // namespace ea::sat

#endif  // EVOL_SOLVER_H
