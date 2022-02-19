#ifndef EVOL_SIMPSOLVER_H
#define EVOL_SIMPSOLVER_H

#include <filesystem>

#include "core/sat/solver/Solver.h"
#include "util/Logger.h"
#include "util/TimeTracer.h"
#include "core/sat/SimpBase.h"

namespace core::sat::solver {

/**
 * @brief Minisat:SimpSolver-based Solver implementation.
 */
class SimpSolver : public Solver, public SimpBase {
 public:
  explicit SimpSolver(SimpSolverConfig const& config);

  void load_problem(Problem const& problem) override;

  State solve(Minisat::vec<Minisat::Lit> const& assumptions) override;

  [[nodiscard]] unsigned num_vars() const noexcept override;

  [[nodiscard]] bool propagate_confl(Minisat::vec<Minisat::Lit> const& assumptions) override;

 private:
  void _do_interrupt() override;

  void _do_clear_interrupt() override;
};

}  // namespace core::sat::solver

#endif  // EVOL_SIMPSOLVER_H
