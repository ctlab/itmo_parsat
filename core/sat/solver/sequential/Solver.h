#ifndef EVOL_SOLVER_H
#define EVOL_SOLVER_H

#include <functional>
#include <filesystem>
#include <memory>
#include <atomic>

#include "core/sat/Problem.h"
#include "core/domain/assignment/Search.h"
#include "core/domain/assignment/FullSearch.h"
#include "core/proto/solve_config.pb.h"
#include "util/Registry.h"
#include "util/Reference.h"
#include "core/sat/SimpBase.h"

#include "core/sat/native/mini/utils/Lit.h"
#include "core/sat/native/mini/mtl/Vec.h"

namespace core::sat::solver {

/**
 * @brief SAT Solver interface.
 */
class Solver {
 public:
  virtual ~Solver() = default;

  /**
   * @brief Parses cnf from .gz file with the specified path.
   * @param path the path with formula.
   */
  virtual void load_problem(Problem const& problem) = 0;

  /**
   * @brief Runs solution with the specified assumptions.
   * @param assumptions assumptions to include.
   * @return the result of solving.
   */
  virtual State solve(lit_vec_t const& assumptions) = 0;

  /**
   * @brief Interrupts solver. Intended to be used from signal handlers.
   */
  virtual void interrupt() = 0;

  /**
   * @brief Clears interrupt flag.
   */
  virtual void clear_interrupt() = 0;

  /**
   * @brief Returns the number of variables in formula.
   */
  [[nodiscard]] virtual unsigned num_vars() const noexcept = 0;
};

MAKE_REFS(Solver);

DEFINE_REGISTRY(Solver, SolverConfig, solver);

}  // namespace core::sat::solver

#endif  // EVOL_SOLVER_H