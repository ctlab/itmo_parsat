#ifndef EVOL_SIMPSOLVER_H
#define EVOL_SIMPSOLVER_H

#include <filesystem>

#include "evol/include/sat/Solver.h"
#include "evol/proto/config.pb.h"
#include "minisat/simp/SimpSolver.h"

namespace ea::sat {

class SimpSolver : public Solver {
 public:
  explicit SimpSolver(SimpSolverConfig const& config);

  void parse_cnf(std::filesystem::path const& path) override;

  [[nodiscard]] State state() const noexcept override;

  void solve_limited(Minisat::vec<Minisat::Lit> const& assumptions) override;

  void interrupt() override;

  [[nodiscard]] unsigned num_vars() const noexcept override;

  [[nodiscard]] bool propagate(
      Minisat::vec<Minisat::Lit> const& assumptions,
      Minisat::vec<Minisat::Lit>& propagated) override;

 private:
  Minisat::SimpSolver impl_;
  State state_ = UNKNOWN;
  bool preprocess_;
};

}  // namespace ea::sat

#endif  // EVOL_SIMPSOLVER_H
