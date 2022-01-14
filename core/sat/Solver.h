#ifndef EVOL_SOLVER_H
#define EVOL_SOLVER_H

#include <functional>
#include <filesystem>
#include <memory>
#include <atomic>

#include "minisat/core/SolverTypes.h"
#include "minisat/mtl/Vec.h"
#include "core/domain/assignment/Search.h"
#include "core/proto/solve_config.pb.h"
#include "core/util/Registry.h"

#ifndef DISABLE_CHECK_ASGN

#include <vector>
#include <mutex>
#include <set>

#endif

namespace core::sat {

enum State {
  SAT,
  UNSAT,
  UNKNOWN,
};

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
   * @brief Parses cnf from .gz file with the specified path.
   * @param path the path with formula.
   */
  virtual void parse_cnf(std::filesystem::path const& path) = 0;

  /**
   * @brief Equivalent to Solver::solve_limited({}).
   * @return the result of solving.
   */
  State solve_limited();

  /**
   * @brief Runs solutions with the specified assumptions.
   * @note Currently it is solve, not solveLimited!
   * @param assumptions assumptions to include.
   * @return the result of solving.
   */
  virtual State solve_limited(Minisat::vec<Minisat::Lit> const& assumptions) = 0;

  /**
   */

  /**
   * @brief Propagates a given list of assumptions.
   * @return true iff conflicts happened.
   * @param assumptions assumptions to include.
   * @param propagated the propagated literals.
   * @return true if and only if conflict occurred.
   */
  virtual bool propagate(
      Minisat::vec<Minisat::Lit> const& assumptions, Minisat::vec<Minisat::Lit>& propagated) = 0;

  /**
   * @brief Propagates a given list of assumptions.
   * @param assumptions assumptions to include.
   * @return true if and only if conflict occurred.
   */
  [[nodiscard]] bool propagate(Minisat::vec<Minisat::Lit> const& assumptions);

  /**
   * @brief Solves CNF on all assignments by the given iterator and calls callback respectively.
   * @param search search engine.
   * @param callback callback to be called on each solve.
   */
  virtual void solve_assignments(domain::USearch search, slv_callback_t const& callback);

  /**
   * @brief Propagates all assignments by the given iterator and calls callback respectively.
   * @param search search engine.
   * @param callback callback to be called on each solve.
   */
  virtual void prop_assignments(domain::USearch search, prop_callback_t const& callback);

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

 protected:
  virtual void _do_interrupt(){};

  virtual void _do_clear_interrupt(){};

 private:
  std::atomic_bool _interrupted{false};
};

using RSolver = std::shared_ptr<Solver>;

using USolver = std::unique_ptr<Solver>;

DEFINE_REGISTRY(Solver, SolverConfig, solver);

#ifndef DISABLE_CHECK_ASGN

#define START_ASGN_TRACK(expected_size)    \
  std::mutex _m_asgn_track;                \
  std::atomic_bool _stop_callback{false};  \
  uint32_t _expected_size = expected_size; \
  std::set<std::vector<bool>> _uniq_asgn

#define ASGN_TRACK(asgn)                            \
  do {                                              \
    std::vector<bool> _cur(asgn.size());            \
    for (int i = 0; i < asgn.size(); ++i) {         \
      _cur[i] = Minisat::sign(asgn[i]);             \
    }                                               \
    std::lock_guard<std::mutex> _lg(_m_asgn_track); \
    _uniq_asgn.insert(std::move(_cur));             \
  } while (0)

#define BREAK_ASGN_TRACK _stop_callback = true

#define END_ASGN_TRACK                                                            \
  IPS_VERIFY(                                                                     \
      (_stop_callback || interrupted() || _uniq_asgn.size() == _expected_size) && \
      bool("Unexpected assignments behaviour"))

#else

#define START_ASGN_TRACK(...)

#define ASGN_TRACK(...)

#define BREAK_ASGN_TRACK

#define END_ASGN_TRACK

#endif

}  // namespace core::sat

#endif  // EVOL_SOLVER_H