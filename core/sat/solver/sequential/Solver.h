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
#include "core/domain/assignment/TrackAssignments.h"
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
  typedef Mini::vec<Mini::Lit> vec_lit_t;

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
  virtual State solve(vec_lit_t const& assumptions) = 0;

  /**
   * @brief Interrupts solver. Intended to be used from signal handlers.
   */
  virtual void interrupt();

  /**
   * @brief Clears interrupt flag.
   */
  virtual void clear_interrupt();

  /**
   * @brief Returns the number of variables in formula.
   */
  [[nodiscard]] virtual unsigned num_vars() const noexcept = 0;
};

DEFINE_REGISTRY(Solver, SolverConfig, solver);

MAKE_REFS(Solver);

}  // namespace core::sat::solver

#endif  // EVOL_SOLVER_H