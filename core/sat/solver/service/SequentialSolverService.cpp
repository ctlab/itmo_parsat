#include "SequentialSolverService.h"

namespace core::sat::solver {

void SequentialSolverService::_solver_working_thread(Solver& solver) {
  while (!_stop) {
    std::unique_lock<std::mutex> ul(_queue_m);
    _queue_cv.wait(ul, [this] { return _stop || !_task_queue.empty(); });
    if (IPS_UNLIKELY(_stop)) {
      break;
    }
    if (IPS_UNLIKELY(_task_queue.empty())) {
      continue;
    }
    auto task = std::move(_task_queue.front());
    _task_queue.pop();
    ul.unlock();
    task(solver);
  }
}

SequentialSolverService::SequentialSolverService(
    SequentialSolverServiceConfig const& config) {
  for (int i = 0; i < config.num_solvers(); ++i) {
    _solvers.emplace_back(
        USolver(SolverRegistry::resolve(config.solver_config())));
  }
  for (int i = 0; i < config.num_solvers(); ++i) {
    _threads.emplace_back([this, i] { _solver_working_thread(*_solvers[i]); });
  }
}

std::future<State> SequentialSolverService::solve(
    lit_vec_t const& assumptions, clock_t::duration time_limit) {
  auto p_task = std::packaged_task<State(Solver&)>(
      [this, assumptions, time_limit](auto& solver) {
        if (time_limit == DUR_INDEF) {
          return IPS_TRACE_N_V(
              "SeqSolverService::solve", solver.solve(assumptions));
        } else {
          return IPS_TRACE_N_V("SeqSolverService::solve_tl",
                               _timer.launch(
                                   [&] { return solver.solve(assumptions); },
                                   [&] { solver.interrupt(); }, time_limit););
        }
      });
  auto future = p_task.get_future();
  {
    std::lock_guard<std::mutex> lg(_queue_m);
    _task_queue.emplace(std::move(p_task));
  }
  _queue_cv.notify_one();
  return future;
}

SequentialSolverService::~SequentialSolverService() noexcept {
  {
    std::lock_guard<std::mutex> lg(_queue_m);
    _stop = true;
    _task_queue = {};
  }
  std::for_each(
      IPS_EXEC_POLICY, _solvers.begin(), _solvers.end(),
      [](auto& solver) { solver->interrupt(); });
  _queue_cv.notify_all();
  for (auto& thread : _threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}

void SequentialSolverService::load_problem(Problem const& problem) {
  std::for_each(
      IPS_EXEC_POLICY, _solvers.begin(), _solvers.end(),
      [&problem](auto& solver) { solver->load_problem(problem); });
}

void SequentialSolverService::interrupt() {
  {
    std::lock_guard<std::mutex> lg(_queue_m);
    _task_queue = {};
  }
  std::for_each(
      IPS_EXEC_POLICY, _solvers.begin(), _solvers.end(),
      [](auto& solver) { solver->interrupt(); });
}

void SequentialSolverService::clear_interrupt() {
  std::for_each(
      IPS_EXEC_POLICY, _solvers.begin(), _solvers.end(),
      [](auto& solver) { solver->clear_interrupt(); });
}

REGISTER_PROTO(
    SolverService, SequentialSolverService, sequential_solver_service_config);

}  // namespace core::sat::solver
