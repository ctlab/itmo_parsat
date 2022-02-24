#ifndef ITMO_PARSAT_PAINLESSSOLVER_H
#define ITMO_PARSAT_PAINLESSSOLVER_H

#include <vector>

#include "core/sat/solver/Solver.h"
#include "core/sat/native/painless/painless-src/solvers/SolverInterface.h"
#include "core/sat/native/painless/painless-src/working/Portfolio.h"
#include "core/sat/native/painless/painless-src/sharing/Sharer.h"
#include "core/sat/native/painless/painless-src/sharing/HordeSatSharing.h"
#include "core/sat/native/painless/painless-src/solvers/SolverFactory.h"
#include "core/sat/native/painless/painless-src/working/SequentialWorker.h"
#include "core/sat/native/painless/painless-src/clauses/ClauseManager.h"
#include "core/sat/native/painless/painless-src/utils/System.h"

namespace core::sat::solver {

class PainlessSolver : public Solver {
 public:
  explicit PainlessSolver(PainlessSolverConfig const& config);

  ~PainlessSolver() noexcept override;

  void load_problem(Problem const& problem) override;

  State solve(vec_lit_t const& assumptions) override;

  [[nodiscard]] unsigned num_vars() const noexcept override;

  [[nodiscard]] bool propagate_confl(vec_lit_t const& assumptions) override;

 private:
  void _do_interrupt() override;

  void _do_clear_interrupt() override;

 private:
  std::vector<SolverInterface*> _solvers;
  std::vector<SolverInterface*> _solvers_VSIDS;
  std::vector<SolverInterface*> _solvers_LRB;

 private:
  vector<SolverInterface*> prod1;
  vector<SolverInterface*> prod2;
  vector<SolverInterface*> cons1;
  vector<SolverInterface*> cons2;
  vector<Sharer*> sharers;

 private:
  PainlessSolverConfig _cfg;
  std::unique_ptr<WorkingStrategy> working = nullptr;
  int nSharers = 0;
};

}  // namespace core::sat::solver

#endif  // ITMO_PARSAT_PAINLESSSOLVER_H
