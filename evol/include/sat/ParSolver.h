#ifndef EVOL_PARSOLVER_H
#define EVOL_PARSOLVER_H

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/asio/post.hpp>
#include <atomic>

#include "evol/include/sat/Solver.h"
#include "evol/proto/config.pb.h"

namespace ea::sat {

class ParSolver : public Solver {
 private:
  void _do_interrupt() noexcept;

  static void _wait_for_futures(std::vector<std::future<void>>&) noexcept;

 public:
  explicit ParSolver(ParSolverConfig const& config);

  ~ParSolver() noexcept override;

  void parse_cnf(std::filesystem::path const& path) override;

  State solve_limited(Minisat::vec<Minisat::Lit> const& assumptions) override;

  bool propagate(
      Minisat::vec<Minisat::Lit> const& assumptions, Minisat::vec<Minisat::Lit>& propagated) override;

  void solve_assignments(domain::UAssignment assignment, slv_callback_t const& callback) override;

  void prop_assignments(domain::UAssignment assignment, prop_callback_t const& callback) override;

  void interrupt() noexcept override;

  [[nodiscard]] unsigned num_vars() const noexcept override;

 private:
  std::atomic_bool interrupt_{false};
  boost::asio::thread_pool thread_pool_;
  std::vector<sat::RSolver> solvers_;
};

}  // namespace ea::sat

#endif  // EVOL_PARSOLVER_H
