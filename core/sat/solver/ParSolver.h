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

#include "core/util/Generator.h"
#include "core/sat/solver/Solver.h"

namespace core::sat {

/**
 * @brief Parallel solver implementation.
 */
class ParSolver : public Solver {
 private:
  struct req_solve_t {
    domain::RSearch assignment;
    Solver::slv_callback_t callback;
  };

  using task_t = std::variant<req_solve_t>;

 private:
  void _solve(sat::Solver& solver, req_solve_t& req);

  std::future<void> _submit(task_t&& task);

 public:
  explicit ParSolver(ParSolverConfig const& config);

  ~ParSolver() noexcept;

  void parse_cnf(std::filesystem::path const& input) override;

  State solve(Minisat::vec<Minisat::Lit> const& assumptions) override;

  void solve_assignments(domain::USearch assignment, slv_callback_t const& callback) override;

  [[nodiscard]] bool propagate_confl(Minisat::vec<Minisat::Lit> const& assumptions) override;

  [[nodiscard]] unsigned num_vars() const noexcept override;

 private:
  void _do_interrupt() override;

  void _do_clear_interrupt() override;

 private:
  std::vector<std::thread> _t;
  std::vector<sat::RSolver> _solvers;
  std::queue<std::packaged_task<void(sat::Solver&)>> _task_queue;
  std::condition_variable _cv;
  std::mutex _m, _asgn_mutex;
  std::atomic_bool _stop{false};
  bool _solve_finished{false};
};

}  // namespace core::sat

#endif  // EVOL_PARSOLVER_H
