#ifndef EVOL_SOLVER_H
#define EVOL_SOLVER_H

#include <functional>
#include <filesystem>
#include <memory>

#include "minisat/core/SolverTypes.h"
#include "minisat/mtl/Vec.h"
#include "core/domain/Assignment.h"
#include "core/proto/solve_config.pb.h"
#include "core/util/Registry.h"

namespace core::sat {

enum State {
  SAT,
  UNSAT,
  UNKNOWN,
};

class Solver {
 public:
  /**
   * Must be thread safe for some implementations.
   */
  // clang-format off
  typedef std::function<
      bool( // true iff should continue solving
        State, // result of solve_limited
        bool, // true iff solved on propagate stage
        Minisat::vec<Minisat::Lit> const& // assumptions passed to solve_limited
      )> slv_callback_t;
  typedef std::function<
      bool( // true iff should continue solving
        bool, // true iff there's been conflict
        Minisat::vec<Minisat::Lit> const&, // assumptions passed to propagate
        Minisat::vec<Minisat::Lit> const& // propagated literals
      )> prop_callback_t; //
  // clang-format on

 public:
  virtual ~Solver() = default;

  /**
   * Parses cnf from .gz file with the specified path.
   */
  virtual void parse_cnf(std::filesystem::path const& path) = 0;

  /**
   * Equivalent to `solve_limited({})`.
   */
  State solve_limited();

  /**
   * Runs solutions with the specified assumptions.
   */
  virtual State solve_limited(Minisat::vec<Minisat::Lit> const& assumptions) = 0;

  /**
   * Propagates a given list of assumptions.
   * Returns true iff conflicts happened.
   */
  virtual bool propagate(
      Minisat::vec<Minisat::Lit> const& assumptions, Minisat::vec<Minisat::Lit>& propagated) = 0;

  /**
   * Propagates a given list of assumptions.
   */
  [[nodiscard]] bool propagate(Minisat::vec<Minisat::Lit> const& assumptions);

  /**
   * Solves CNF on all assignments by the given iterator and calls callback respectively.
   */
  virtual void solve_assignments(domain::UAssignment assignment_p, slv_callback_t const& callback);

  /**
   * Propagates all assignments by the given iterator and calls callback respectively.
   */
  virtual void prop_assignments(domain::UAssignment assignment_p, prop_callback_t const& callback);

  /**
   * Interrupts solver. Intended to be used from signal handlers.
   */
  virtual void interrupt() = 0;

  /**
   * Returns true if interrupt flag is set.
   */
  [[nodiscard]] virtual bool interrupted() const = 0;

  /**
   * Returns the number of variables in formula.
   */
  [[nodiscard]] virtual unsigned num_vars() const noexcept = 0;
};

using RSolver = std::shared_ptr<Solver>;

using USolver = std::unique_ptr<Solver>;

DEFINE_REGISTRY(Solver, SolverConfig, solver);

}  // namespace core::sat

#endif  // EVOL_SOLVER_H