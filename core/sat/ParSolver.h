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
#include "core/sat/Solver.h"

namespace core::sat {

/**
 * @brief Parallel solver implementation.
 */
class ParSolver : public Solver {
 private:
  struct req_prop_t {
    domain::USearch assignment;
    Solver::prop_callback_t callback;
  };

  struct req_solve_t {
    domain::RSearch assignment;
    Solver::slv_callback_t callback;
    int idx;
  };

  using task_t = std::variant<req_prop_t, req_solve_t>;

 private:
  void _solve(sat::Solver& solver, req_solve_t& req);

  void _propagate(sat::Solver& solver, req_prop_t& req);

  std::future<void> _submit(task_t&& task);

 public:
  explicit ParSolver(ParSolverConfig const& config);

  ~ParSolver() noexcept;

  void parse_cnf(std::filesystem::path const& input) override;

  State solve_limited(Minisat::vec<Minisat::Lit> const& assumptions) override;

  bool propagate(
      Minisat::vec<Minisat::Lit> const& assumptions,
      Minisat::vec<Minisat::Lit>& propagated) override;

  void solve_assignments(domain::USearch assignment, slv_callback_t const& callback) override;

  void prop_assignments(domain::USearch assignment, prop_callback_t const& callback) override;

  [[nodiscard]] unsigned num_vars() const noexcept override;

 private:
  void _do_interrupt() override;

  void _do_clear_interrupt() override;

 private:
  std::vector<std::thread> _t;
  std::vector<sat::RSolver> _solvers;
  std::queue<std::packaged_task<void(sat::Solver&)>> _task_queue;
  std::condition_variable _cv;
  std::mutex _m, _asgn_mutex, _slv_mutex;
  std::atomic_bool _stop{false};
};

}  // namespace core::sat

#endif  // EVOL_PARSOLVER_H
