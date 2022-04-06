#ifndef ITMO_PARSAT_PAINLESSSOLVER_H
#define ITMO_PARSAT_PAINLESSSOLVER_H

#include <vector>

#include "core/sat/sharing/Sharing.h"
#include "core/sat/solver/sequential/Solver.h"
#include "core/sat/native/painless/painless-src/solvers/SolverInterface.h"
#include "core/sat/native/painless/painless-src/working/Portfolio.h"
#include "core/sat/native/painless/painless-src/sharing/Sharer.h"
#include "core/sat/native/painless/painless-src/sharing/HordeSatSharing.h"
#include "core/sat/native/painless/painless-src/solvers/SolverFactory.h"
#include "core/sat/native/painless/painless-src/working/SequentialWorker.h"
#include "core/sat/native/painless/painless-src/clauses/ClauseManager.h"
#include "core/sat/native/painless/painless-src/utils/System.h"

namespace core::sat::solver {

/**
 * @brief Painless-based Solver implementation
 */
class PainlessSolver : public Solver {
 public:
  explicit PainlessSolver(PainlessSolverConfig config);

  ~PainlessSolver() noexcept override;

  void load_problem(Problem const& problem) override;

  State solve(lit_vec_t const& assumptions) override;

  [[nodiscard]] unsigned num_vars() const noexcept override;

  void interrupt() override;

  void clear_interrupt() override;

  sharing::SharingUnit sharing_unit() noexcept override;

 private:
  sharing::SolverBlockList _solver_block_list;
  sharing::Sharing _sharing;
  std::vector<SolverInterface*> _solvers;
  PainlessSolverConfig _cfg;
  painless::WorkingResult _result;
  std::unique_ptr<painless::WorkingStrategy> working;
  int64_t solve_index = 0;
  int nSharers = 0;
  bool _interrupted = false;
};

}  // namespace core::sat::solver

#endif  // ITMO_PARSAT_PAINLESSSOLVER_H
