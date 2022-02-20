#ifndef ITMO_PARSAT_HORDESATSOLVER_H
#define ITMO_PARSAT_HORDESATSOLVER_H

#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>

#include "core/sat/solver/Solver.h"
#include "core/sat/prop/ParProp.h"
#include "util/Registry.h"

#include "core/sat/native/hordesat/hordesat-src/solvers/MiniSat.h"
#include "core/sat/native/hordesat/hordesat-src/solvers/Lingeling.h"
#include "core/sat/native/hordesat/hordesat-src/utilities/DebugUtils.h"
#include "core/sat/native/hordesat/hordesat-src/utilities/Logger.h"
#include "core/sat/native/hordesat/hordesat-src/utilities/Threading.h"
#include "core/sat/native/hordesat/hordesat-src/utilities/ParameterProcessor.h"
#include "core/sat/native/hordesat/hordesat-src/sharing/AllToAllSharingManager.h"
#include "core/sat/native/hordesat/hordesat-src/sharing/LogSharingManager.h"

namespace core::sat::solver {

class HordeSatSolver : public Solver {
 public:
  explicit HordeSatSolver(HordeSatSolverConfig const& config);

  ~HordeSatSolver() noexcept override;

  void load_problem(Problem const& problem) override;

  State solve(Minisat::vec<Minisat::Lit> const& assumptions) override;

  void solve_assignments(domain::USearch search, Solver::slv_callback_t const& callback) override;

  [[nodiscard]] unsigned num_vars() const noexcept override;

  [[nodiscard]] bool propagate_confl(Minisat::vec<Minisat::Lit> const& assumptions) override;

 private:
  void _diversify();

  void _do_interrupt() override;

  void _do_clear_interrupt() override;

 private:
  State _result;
  uint32_t _num_solvers;

  std::vector<std::unique_ptr<PortfolioSolverInterface>> _solvers;
  std::vector<PortfolioSolverInterface*> _solver_ptrs;
  std::vector<std::thread> _solver_threads;

  bool _solvingDoneLocal = false;
  bool _interrupted = false;

  std::mutex _interrupt_lock;
  std::mutex _solve_mutex;
  std::mutex _slv_mutex;

  std::condition_variable _slv_cv;
  std::vector<int> _assumptions;
  uint64_t _solve_idx = 0;
  bool _stop = false;

  prop::RProp _prop;
  HordeSatSolverConfig _cfg;
  std::unique_ptr<SharingManagerInterface> _sharing_manager;
};

}  // namespace core::sat::solver

#endif  // ITMO_PARSAT_HORDESATSOLVER_H
