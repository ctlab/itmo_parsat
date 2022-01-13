#include "core/sat/ParSolver.h"

#include <filesystem>
#include <mutex>

namespace {

template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};

template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

void _wait_for_futures(std::vector<std::future<void>>& futures) noexcept {
  for (auto& future : futures) {
    future.get();
  }
}

}  // namespace

namespace core::sat {

ParSolver::ParSolver(ParSolverConfig const& config) {
  for (uint32_t i = 0; i < config.max_threads(); ++i) {
    _solvers.emplace_back(SolverRegistry::resolve(config.solver_config()));
  }
  auto& parent_generator = core::Generator::current_thread_generator();

  for (uint32_t tid = 0; tid < config.max_threads(); ++tid) {
    _t.emplace_back([this, tid, &parent_generator] {
      auto& solver = *_solvers[tid];
      core::Generator child_generator(parent_generator);

      while (!_stop) {
        std::unique_lock<std::mutex> ul(_m);
        _cv.wait(ul, [this] { return _stop || !_task_queue.empty(); });
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
    });
  }
}

ParSolver::~ParSolver() noexcept {
  interrupt();
  {
    std::lock_guard<std::mutex> lg(_m);
    _stop = true;
  }
  _cv.notify_all();

  for (auto& t : _t) {
    if (t.joinable()) {
      t.join();
    }
  }
}

void ParSolver::parse_cnf(std::filesystem::path const& input) {
  for (auto& solver : _solvers) {
    solver->parse_cnf(input);
  }
}

State ParSolver::solve_limited(Minisat::vec<Minisat::Lit> const& assumptions) {
  return _solvers.front()->solve_limited(assumptions);
}

bool ParSolver::propagate(
    Minisat::vec<Minisat::Lit> const& assumptions, Minisat::vec<Minisat::Lit>& propagated) {
  return _solvers.front()->propagate(assumptions, propagated);
}

void ParSolver::solve_assignments(
    domain::UAssignment assignment_p, Solver::slv_callback_t const& callback) {
  clear_interrupt();
  size_t num_threads = _t.size();

  // Here we share assignment between all worker threads.
  // Don't forget to release unique ptr.
  domain::RAssignment shared_assignment(assignment_p.get());
  assignment_p.release();

  std::vector<std::future<void>> futures;
  futures.reserve(num_threads);
  for (uint32_t index = 0; index < num_threads; ++index) {
    futures.push_back(_submit(req_solve_t{shared_assignment, callback, (int) index}));
  }
  _wait_for_futures(futures);
}

void ParSolver::prop_assignments(
    domain::UAssignment assignment_p, Solver::prop_callback_t const& callback) {
  clear_interrupt();
  uint32_t num_threads = _t.size();

  std::vector<std::future<void>> futures;
  futures.reserve(num_threads);
  for (uint32_t index = 0; index < num_threads; ++index) {
    futures.push_back(
        _submit(req_prop_t{assignment_p->split_search(num_threads, index), callback}));
  }
  _wait_for_futures(futures);
}

void ParSolver::_solve(sat::Solver& solver, req_solve_t& req) {
  START_ASGN_TRACK(req.assignment->size());
  if (!req.assignment->empty()) {
    auto& assignment = *req.assignment;
    Minisat::vec<Minisat::Lit> arg(assignment().size());
    bool stop = false;
    do {
      {
        std::lock_guard<std::mutex> asgn_lg(_asgn_mutex);
        if (interrupted()) {
          break;
        }
        for (int i = 0; i < assignment().size(); ++i) {
          arg[i] = assignment()[i];
        }
        if (!++assignment) {
          interrupt();
          stop = true;
        }
      }
      ASGN_TRACK(arg);
      bool conflict;
      State result;
      {
        //        std::lock_guard<std::mutex> lg(_slv_mutex);
        conflict = solver.propagate(arg);
        IPS_LOG_IF(INFO, !conflict, "Solver #" << req.idx);
        result = conflict ? UNSAT : solver.solve_limited(arg);
      }
      if (!req.callback(result, conflict, arg)) {
        BREAK_ASGN_TRACK;
        break;
      }
    } while (!IPS_UNLIKELY(interrupted() && !stop));
  }
  END_ASGN_TRACK;
}

void ParSolver::_propagate(sat::Solver& solver, req_prop_t& req) {
  START_ASGN_TRACK(req.assignment->size());
  if (!req.assignment->empty()) {
    auto& assignment = *req.assignment;
    Minisat::vec<Minisat::Lit> propagated;
    do {
      ASGN_TRACK(assignment());
      bool result = solver.propagate(assignment(), propagated);
      if (!req.callback(result, assignment(), propagated)) {
        BREAK_ASGN_TRACK;
        break;
      }
    } while (!IPS_UNLIKELY(interrupted()) && ++assignment);
  }
  END_ASGN_TRACK;
}

std::future<void> ParSolver::_submit(task_t&& task) {
  // clang-format off
  auto p_task = std::packaged_task<void(sat::Solver&)>(
    [this, task = std::move(task)] (sat::Solver& solver) mutable {
      std::visit(overloaded{
          [&solver, this](req_solve_t& req) {
            _solve(solver, req);
          },
          [&solver, this](req_prop_t& req) {
            _propagate(solver, req);
          }
      }, task);
  });
  // clang-format on
  auto future = p_task.get_future();
  {
    std::lock_guard<std::mutex> lg(_m);
    _task_queue.emplace(std::move(p_task));
  }
  _cv.notify_one();
  return future;
}

void ParSolver::_do_interrupt() {
  for (auto& solver : _solvers) {
    solver->interrupt();
  }
}

void ParSolver::_do_clear_interrupt() {
  for (auto& solver : _solvers) {
    solver->clear_interrupt();
  }
}

unsigned ParSolver::num_vars() const noexcept {
  return _solvers.front()->num_vars();
}

REGISTER_PROTO(Solver, ParSolver, par_solver_config);

}  // namespace core::sat