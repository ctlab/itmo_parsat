#ifndef EVOL_SIMPSOLVER_H
#define EVOL_SIMPSOLVER_H

#include <filesystem>

#include "core/sat/Solver.h"
#include "minisat/simp/SimpSolver.h"

namespace core::sat {

class SimpSolver : public Solver, public Minisat::SimpSolver {
 public:
  explicit SimpSolver(SimpSolverConfig const& config);

  void parse_cnf(std::filesystem::path const& path) override;

  State solve_limited(Minisat::vec<Minisat::Lit> const& assumptions) override;

  void interrupt() override;

  [[nodiscard]] bool is_interrupted() const override;

  [[nodiscard]] unsigned num_vars() const noexcept override;

  [[nodiscard]] bool propagate(
      Minisat::vec<Minisat::Lit> const& assumptions,
      Minisat::vec<Minisat::Lit>& propagated) override;

 private:
  bool preprocess_;
};

}  // namespace core::sat

#endif  // EVOL_SIMPSOLVER_H
