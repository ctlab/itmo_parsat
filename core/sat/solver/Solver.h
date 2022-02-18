#ifndef EVOL_SOLVER_H
#define EVOL_SOLVER_H

#include <functional>
#include <filesystem>
#include <memory>
#include <atomic>

#include "minisat/core/SolverTypes.h"
#include "minisat/mtl/Vec.h"
#include "util/Registry.h"
#include "core/sat/Problem.h"
#include "core/domain/assignment/Search.h"
#include "core/domain/assignment/FullSearch.h"
#include "core/proto/solve_config.pb.h"
#include "core/domain/assignment/TrackAssignments.h"

namespace core::sat::solver {

/**
 * @brief SAT Solver interface.
 */
class Solver {
 public:
  /**
   * @details Both callbacks types must be thread safe for asynchronous implementations.
   */
  // clang-format off
  typedef std::function<
      bool( // true iff should continue solving
        State, // result of solve
        bool, // true iff solved on propagate stage
        Minisat::vec<Minisat::Lit> const& // assumptions passed to solve
      )> slv_callback_t;
  // clang-format on

 public:
  virtual ~Solver() = default;

  /**
   * @brief Parses cnf from .gz file with the specified path.
   * @param path the path with formula.
   */
  virtual void load_problem(Problem const& problem) = 0;

  /**
   * @brief Equivalent to Solver::solve({}).
   * @return the result of solving.
   */
  State solve_limited();

  /**
   * @brief Runs solution with the specified assumptions.
   * @param assumptions assumptions to include.
   * @return the result of solving.
   */
  virtual State solve(Minisat::vec<Minisat::Lit> const& assumptions) = 0;

  /**
   * @brief Solves CNF on all assignments by the given iterator and calls callback respectively.
   * @param search search engine.
   * @param callback callback to be called on each solve.
   */
  virtual void solve_assignments(domain::USearch search, slv_callback_t const& callback);

  /**
   * @brief Interrupts solver. Intended to be used from signal handlers.
   */
  void interrupt();

  /**
   * @brief Clears interrupt flag.
   */
  void clear_interrupt();

  /**
   * @brief Returns true if interrupt flag is set.
   */
  [[nodiscard]] bool interrupted() const;

  /**
   * @brief Returns the number of variables in formula.
   */
  [[nodiscard]] virtual unsigned num_vars() const noexcept = 0;

  /**
   * @param assumptions assumptions to be passed to solver
   * @return whether these assumptions lead to conflict
   */
  [[nodiscard]] virtual bool propagate_confl(Minisat::vec<Minisat::Lit> const& assumptions) = 0;

 protected:
  virtual void _do_interrupt(){};

  virtual void _do_clear_interrupt(){};

 private:
  std::atomic_bool _interrupted{false};
};

using RSolver = std::shared_ptr<Solver>;

using USolver = std::unique_ptr<Solver>;

DEFINE_REGISTRY(Solver, SolverConfig, solver);

}  // namespace core::sat::solver

#endif  // EVOL_SOLVER_H