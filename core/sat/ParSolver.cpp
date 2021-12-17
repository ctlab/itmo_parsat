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

}  // namespace

namespace core::sat {

ParSolver::ParSolver(ParSolverConfig const& config) : _solver_pool(config) {}

ParSolver::~ParSolver() noexcept {
  _solver_pool.interrupt();
}

void ParSolver::parse_cnf(std::filesystem::path const& input) {
  _solver_pool.parse_cnf(input);
}

State ParSolver::solve_limited(Minisat::vec<Minisat::Lit> const& assumptions) {
  return _solver_pool.get_solver()->solve_limited(assumptions);
}

bool ParSolver::propagate(
    Minisat::vec<Minisat::Lit> const& assumptions, Minisat::vec<Minisat::Lit>& propagated) {
  return _solver_pool.get_solver()->propagate(assumptions, propagated);
}

void ParSolver::solve_assignments(
    domain::UAssignment assignment_p, Solver::slv_callback_t const& callback) {
  _solver_pool.reset_interrupt();
  size_t num_threads = _solver_pool.num_threads();

  // Here we share assignment between all worker threads.
  // Don't forget to release unique ptr.
  domain::RAssignment shared_assignment(assignment_p.get());
  assignment_p.release();

  std::vector<std::future<void>> futures;
  futures.reserve(num_threads);
  for (uint32_t index = 0; index < num_threads; ++index) {
    futures.push_back(
        _solver_pool.submit(_details::SolverPool::req_solve_t{shared_assignment, callback}));
  }
  _wait_for_futures(futures);
}

void ParSolver::prop_assignments(
    domain::UAssignment assignment_p, Solver::prop_callback_t const& callback) {
  _solver_pool.reset_interrupt();
  uint32_t num_threads = _solver_pool.num_threads();

  std::vector<std::future<void>> futures;
  futures.reserve(num_threads);
  for (uint32_t index = 0; index < num_threads; ++index) {
    futures.push_back(_solver_pool.submit(_details::SolverPool::req_prop_t{
        assignment_p->split_search(num_threads, index), callback}));
  }
  _wait_for_futures(futures);
}

void ParSolver::_wait_for_futures(std::vector<std::future<void>>& futures) noexcept {
  for (auto& future : futures) {
    future.get();
  }
}

void ParSolver::interrupt() noexcept {
  _solver_pool.interrupt();
}

bool ParSolver::interrupted() const {
  return _solver_pool.interrupted();
}

unsigned ParSolver::num_vars() const noexcept {
  return _solver_pool.get_solver()->num_vars();
}

REGISTER_PROTO(Solver, ParSolver, par_solver_config);

namespace _details {

SolverPool::req_prop_t::req_prop_t(domain::UAssignment assignment, Solver::prop_callback_t callback)
    : assignment(std::move(assignment)), callback(std::move(callback)) {}

SolverPool::req_solve_t::req_solve_t(
    domain::RAssignment assignment, Solver::slv_callback_t callback)
    : assignment(std::move(assignment)), callback(std::move(callback)) {}

SolverPool::SolverPool(ParSolverConfig const& config) {
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
        if (_task_queue.empty()) {
          continue;
        }
        if (IPS_UNLIKELY(_stop)) {
          break;
        }

        auto task = std::move(_task_queue.front());
        _task_queue.pop();
        ul.unlock();

        // clang-format off
        std::visit(
          overloaded{
            [&solver, this](req_prop_t& x) {
              _propagate(solver, x);
            },
            [&solver, this](req_solve_t& x) {
              _solve(solver, x);
            }
          }, task);
        // clang-format on
      }
    });
  }
}

SolverPool::~SolverPool() {
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

void SolverPool::interrupt() noexcept {
  _interrupt = true;
  for (auto& solver : _solvers) {
    solver->interrupt();
  }
}

std::future<void> SolverPool::submit(task_t&& task) {
  auto future = std::visit([](auto& req) { return req._promise.get_future(); }, task);
  {
    std::lock_guard<std::mutex> lg(_m);
    _task_queue.emplace(std::move(task));
  }
  _cv.notify_all();
  return future;
}

void SolverPool::parse_cnf(std::filesystem::path const& input) {
  for (auto& solver : _solvers) {
    solver->parse_cnf(input);
  }
}

void SolverPool::_solve(sat::Solver& solver, req_solve_t& req) {
  if (!req.assignment->is_empty()) {
    auto& assignment = *req.assignment;
    Minisat::vec<Minisat::Lit> arg(assignment().size());
    bool stop = false;
    do {
      {
        std::lock_guard<std::mutex> asgn_lg(_asgn_mutex);
        if (_interrupt) {
          break;
        }
        for (int i = 0; i < assignment().size(); ++i) {
          arg[i] = assignment()[i];
        }
        if (!++assignment) {
          _interrupt = true;
          stop = true;
        }
      }
      bool conflict = solver.propagate(arg);
      State result = conflict ? UNSAT : solver.solve_limited(arg);
      if (!req.callback(result, conflict, arg)) {
        break;
      }
    } while (!_interrupt && !stop);
    req._promise.set_value();
  }
}

void SolverPool::_propagate(sat::Solver& solver, req_prop_t& req) {
  if (!req.assignment->is_empty()) {
    auto& assignment = *req.assignment;
    Minisat::vec<Minisat::Lit> propagated;
    do {
      bool result = solver.propagate(assignment(), propagated);
      if (!req.callback(result, assignment(), propagated)) {
        break;
      }
    } while (!_interrupt && ++assignment);
  }
  req._promise.set_value();
}

sat::RSolver& SolverPool::get_solver() noexcept {
  return _solvers[0];
}

sat::RSolver const& SolverPool::get_solver() const noexcept {
  return _solvers[0];
}

size_t SolverPool::num_threads() const noexcept {
  return _solvers.size();
}

bool SolverPool::interrupted() const noexcept {
  return _interrupt;
}

void SolverPool::reset_interrupt() noexcept {
  _interrupt = false;
}

}  // namespace _details
}  // namespace core::sat