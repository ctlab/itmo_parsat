#ifndef EVOL_PARSOLVER_H
#define EVOL_PARSOLVER_H

#include <vector>
#include <atomic>
#include <variant>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <queue>

#include "util/Random.h"
#include "core/sat/solver/Solver.h"
#include "util/WorkerPool.h"

namespace core::sat::solver {

/**
 * @brief Parallel solver implementation.
 */
class ParSolver : public Solver {
 private:
  void _solve(Solver& solver, domain::RSearch search, slv_callback_t const& callback);

 public:
  explicit ParSolver(ParSolverConfig const& config);

  ~ParSolver() noexcept = default;

  void load_problem(Problem const& problem) override;

  State solve(Minisat::vec<Minisat::Lit> const& assumptions) override;

  void solve_assignments(domain::USearch assignment, slv_callback_t const& callback) override;

  [[nodiscard]] bool propagate_confl(Minisat::vec<Minisat::Lit> const& assumptions) override;

  [[nodiscard]] unsigned num_vars() const noexcept override;

 private:
  void _do_interrupt() override;

  void _do_clear_interrupt() override;

 private:
  std::mutex _asgn_mutex;
  bool _solve_finished{false};

 private:
  using SolverWorkerPool = util::WorkerPool<RSolver>;
  SolverWorkerPool _solver_pool;
};

}  // namespace core::sat::solver

#endif  // EVOL_PARSOLVER_H
