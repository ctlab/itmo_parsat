#include "evol/include/sat/ParSolver.h"

#include "evol/include/util/Registry.h"
#include "evol/include/util/random.h"

#include <glog/logging.h>

namespace ea::sat {

ParSolver::ParSolver(ParSolverConfig const& config)
    : thread_pool_(config.max_threads()), solver_mutexes_(config.max_threads()) {
  for (uint32_t i = 0; i < config.max_threads(); ++i) {
    solvers_.push_back(registry::Registry::resolve_solver(config.solver_type()));
  }
}

ParSolver::~ParSolver() noexcept {
  _do_interrupt();
  thread_pool_.join();
  thread_pool_.stop();
}

void ParSolver::parse_cnf(std::filesystem::path const& path) {
  for (auto& solver : solvers_) {
    solver->parse_cnf(path);
  }
}

State ParSolver::solve_limited(Minisat::vec<Minisat::Lit> const& assumptions) {
  return solvers_.front()->solve_limited(assumptions);
}

bool ParSolver::propagate(
    Minisat::vec<Minisat::Lit> const& assumptions, Minisat::vec<Minisat::Lit>& propagated) {
  return solvers_.front()->propagate(assumptions, propagated);
}

void ParSolver::solve_assignments(
    domain::UAssignment assignment_p, Solver::slv_callback_t const& callback) {
  interrupt_ = false;
  std::vector<std::future<void>> futures;
  auto& assignment = *assignment_p;

  uint32_t num_threads = solvers_.size();
  uint32_t num_to_do = assignment.size() / num_threads;
  uint32_t num_overwork = assignment.size() % num_threads;

  for (uint32_t t = 0, index = 0; t < num_threads; ++t) {
    uint32_t to_do = num_to_do + (t < num_overwork);

    // clang-format off
    futures.push_back(boost::asio::post(thread_pool_,
      boost::asio::use_future(
        [this, t, to_do, index, &callback, assumption = domain::UAssignment(assignment.clone())]() {
          auto& solver = *solvers_[t];
          auto& assignment = *assumption;
          assignment.set(index);
          for (uint32_t i = 0; i < to_do && !interrupt_; ++i, ++assignment) {
            auto const& arg = assignment();
            State result = solver.solve_limited(arg);
            if (!callback(result, arg)) {
              break;
            }
          }
        })));
    // clang-format on

    index += to_do;
  }

  _wait_for_futures(futures);
}

void ParSolver::prop_assignments(
    domain::UAssignment assignment_p, Solver::prop_callback_t const& callback) {
  interrupt_ = false;
  std::vector<std::future<void>> futures;
  auto& assignment = *assignment_p;

  uint32_t num_threads = solvers_.size();
  uint32_t num_to_do = assignment.size() / num_threads;
  uint32_t num_overwork = assignment.size() % num_threads;

  for (uint32_t t = 0, index = 0; t < num_threads; ++t) {
    uint32_t to_do = num_to_do + (t < num_overwork);

    // clang-format off
    futures.push_back(boost::asio::post(thread_pool_,
        boost::asio::use_future(
          [this, t, to_do, index, &callback, assumption = domain::UAssignment(assignment.clone())]() {
            auto& solver = *solvers_[t];
            auto& assignment = *assumption;
            assignment.set(index);
            Minisat::vec<Minisat::Lit> propagated;
            for (uint32_t i = 0; i < to_do && !interrupt_; ++i, ++assignment) {
              auto const& arg = assignment();
              bool result = solver.propagate(arg, propagated);
              if (!callback(result, arg, propagated)) {
                break;
              }
            }
        })));
    // clang-format on

    index += to_do;
  }

  _wait_for_futures(futures);
}

void ParSolver::_wait_for_futures(std::vector<std::future<void>>& futures) noexcept {
  for (auto& future : futures) {
    future.get();
  }
}

void ParSolver::_do_interrupt() noexcept {
  interrupt_ = true;
  for (auto& solver : solvers_) {
    solver->interrupt();
  }
}

void ParSolver::interrupt() noexcept {
  _do_interrupt();
}

unsigned ParSolver::num_vars() const noexcept {
  return solvers_.front()->num_vars();
}

REGISTER_PROTO(Solver, ParSolver, par_solver_config);

}  // namespace ea::sat