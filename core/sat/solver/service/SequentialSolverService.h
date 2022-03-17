#ifndef ITMO_PARSAT_SEQUENTIALSOLVERSERVICE_H
#define ITMO_PARSAT_SEQUENTIALSOLVERSERVICE_H

#include <vector>
#include <queue>
#include <future>
#include <functional>
#include <thread>

#include "core/proto/solve_config.pb.h"
#include "core/sat/Problem.h"
#include "core/sat/solver/service/SolverService.h"
#include "util/Timer.h"

namespace core::sat::solver {

using namespace std::chrono_literals;

class SequentialSolverService : public SolverService {
 public:
  explicit SequentialSolverService(SequentialSolverServiceConfig const& config);

  ~SequentialSolverService() noexcept override;

  std::future<core::sat::State> solve(
      Mini::vec<Mini::Lit> const& assumptions,
      core::clock_t::duration time_limit = 24h) override;

  void interrupt() override;

  void clear_interrupt() override;

  void load_problem(sat::Problem const& problem) override;

 private:
  void _solver_working_thread(
      core::sat::solver::Solver& solver, util::Timer& timer);

 private:
  std::mutex _queue_m;
  std::condition_variable _queue_cv;
  std::queue<std::packaged_task<void(sat::solver::Solver&, util::Timer&)>>
      _task_queue;

  std::vector<core::sat::solver::USolver> _solvers;
  std::vector<util::Timer> _timers;
  std::vector<std::thread> _threads;
  bool _stop = false;
};

}  // namespace core::sat::solver

#endif  // ITMO_PARSAT_SEQUENTIALSOLVERSERVICE_H
