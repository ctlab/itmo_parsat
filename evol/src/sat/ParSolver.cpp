#include "evol/include/sat/ParSolver.h"

#include "evol/include/util/Registry.h"
#include "evol/include/util/random.h"

#include <mutex>
#include <glog/logging.h>

namespace ea::sat {

ParSolver::ParSolver(ParSolverConfig const& config) : thread_pool_(config.max_threads()) {
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
  uint32_t num_threads = solvers_.size();
  std::mutex asgn_mutex;
  for (uint32_t index = 0; index < num_threads; ++index) {
    // clang-format off
    futures.push_back(boost::asio::post(thread_pool_,
      boost::asio::use_future(
        [this, &asgn_mutex, index, &callback, &assignment_p]() {
          if (!assignment_p->is_empty()) {
            auto& solver = *solvers_[index];
            auto& assignment = *assignment_p;
            Minisat::vec<Minisat::Lit> arg(assignment().size());
            bool stop = false;
            do {
              {
                std::lock_guard<std::mutex> asgn_lg(asgn_mutex);
                for (int i = 0; i < assignment().size(); ++i) {
                  arg[i] = assignment()[i];
                }
                if (!++assignment) {
                  interrupt_ = true;
                  stop = true;
                }
              }
              State result = solver.solve_limited(arg);
              if (!callback(result, arg)) {
                break;
              }
            } while (!interrupt_ && !stop);
          }
        })));
    // clang-format on
  }

  _wait_for_futures(futures);
}

void ParSolver::prop_assignments(
    domain::UAssignment assignment_p, Solver::prop_callback_t const& callback) {
  interrupt_ = false;
  std::vector<std::future<void>> futures;
  uint32_t num_threads = solvers_.size();
  for (uint32_t index = 0; index < num_threads; ++index) {
    // clang-format off
    futures.push_back(boost::asio::post(thread_pool_,
        boost::asio::use_future(
          [this, index, &callback, assumption = assignment_p->split_search(num_threads, index)]() {
            if (!assumption->is_empty()) {
              auto& assignment = *assumption;
              Minisat::vec<Minisat::Lit> propagated;
              do {
                auto const& arg = assignment();
                bool result = solvers_[index]->propagate(arg, propagated);
                if (!callback(result, arg, propagated)) {
                  break;
                }
              } while (!interrupt_ && ++assignment);
            }
          })));
    // clang-format on
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