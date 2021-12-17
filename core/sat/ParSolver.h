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

namespace _details {

class SolverPool {
 public:
  struct req_prop_t {
    domain::UAssignment assignment;
    Solver::prop_callback_t callback;

   public:
    req_prop_t(domain::UAssignment assignment, Solver::prop_callback_t callback);

   private:
    std::promise<void> _promise{};

   private:
    friend class SolverPool;
  };

  struct req_solve_t {
    domain::RAssignment assignment;
    Solver::slv_callback_t callback;

   public:
    req_solve_t(domain::RAssignment assignment, Solver::slv_callback_t callback);

   private:
    std::promise<void> _promise{};

   private:
    friend class SolverPool;
  };

  using task_t = std::variant<req_prop_t, req_solve_t>;

 public:
  SolverPool(ParSolverConfig const& config);

  ~SolverPool();

  void interrupt() noexcept;

  std::future<void> submit(task_t&& task);

  void parse_cnf(std::filesystem::path const& input);

  void reset_interrupt() noexcept;

  sat::RSolver& get_solver() noexcept;

  sat::RSolver const& get_solver() const noexcept;

  [[nodiscard]] size_t num_threads() const noexcept;

  [[nodiscard]] bool interrupted() const noexcept;

 private:
  void _solve(sat::Solver& solver, req_solve_t& req);

  void _propagate(sat::Solver& solver, req_prop_t& req);

 private:
  std::vector<std::thread> _t;
  std::vector<sat::RSolver> _solvers;
  std::queue<task_t> _task_queue;
  std::condition_variable _cv;
  std::mutex _m, _asgn_mutex;
  std::atomic_bool _interrupt{false};
  std::atomic_bool _stop{false};
};

}  // namespace _details

class ParSolver : public Solver {
 private:
  static void _wait_for_futures(std::vector<std::future<void>>&) noexcept;

 public:
  explicit ParSolver(ParSolverConfig const& config);

  ~ParSolver() noexcept override;

  void parse_cnf(std::filesystem::path const& input) override;

  State solve_limited(Minisat::vec<Minisat::Lit> const& assumptions) override;

  bool propagate(
      Minisat::vec<Minisat::Lit> const& assumptions,
      Minisat::vec<Minisat::Lit>& propagated) override;

  void solve_assignments(domain::UAssignment assignment, slv_callback_t const& callback) override;

  void prop_assignments(domain::UAssignment assignment, prop_callback_t const& callback) override;

  void interrupt() noexcept override;

  [[nodiscard]] bool interrupted() const override;

  [[nodiscard]] unsigned num_vars() const noexcept override;

 private:
  _details::SolverPool _solver_pool;
};

}  // namespace core::sat

#endif  // EVOL_PARSOLVER_H
