#ifndef ITMO_PARSAT_SEQUENTIALSOLVERSERVICE_H
#define ITMO_PARSAT_SEQUENTIALSOLVERSERVICE_H

#include <vector>
#include <queue>
#include <future>
#include <functional>
#include <thread>

#include "core/sat/sharing/Sharing.h"
#include "core/proto/solve_config.pb.h"
#include "core/sat/Problem.h"
#include "core/sat/solver/service/SolverService.h"
#include "core/sat/native/painless/painless-src/sharing/Sharer.h"
#include "core/sat/native/painless/painless-src/sharing/HordeSatSharing.h"
#include "util/Timer.h"
#include "util/options.h"

namespace core::sat::solver {

/**
 * @brief The solver service managing multiple sequential solvers.
 */
class SequentialSolverService : public SolverService {
 public:
  explicit SequentialSolverService(SequentialSolverServiceConfig config);

  ~SequentialSolverService() noexcept override;

  std::future<State> solve(
      lit_vec_t const& assumption, util::clock_t::duration time_limit,
      slv_callback_t const& callback) override;

  void load_problem(sat::Problem const& problem) override;

  void interrupt() override;

  void clear_interrupt() override;

  sharing::SharingUnit sharing_unit() noexcept override;

 private:
  void _solver_working_thread(Solver& solver);

  sharing::SharingUnit _sharing_unit() noexcept;

 private:
  bool _stop = false;
  SequentialSolverServiceConfig _cfg;
  util::Timer _timer;
  std::mutex _queue_m;
  std::condition_variable _queue_cv;
  std::queue<std::packaged_task<void(Solver&)>> _task_queue;
  std::vector<USolver> _solvers;
  std::optional<sat::sharing::Sharing> _sharing;
  std::vector<std::thread> _threads;
};

}  // namespace core::sat::solver

#endif  // ITMO_PARSAT_SEQUENTIALSOLVERSERVICE_H
