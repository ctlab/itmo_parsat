#ifndef EVOL_PROP_H
#define EVOL_PROP_H

#include <functional>
#include <filesystem>
#include <memory>
#include <atomic>

#include "minisat/core/SolverTypes.h"
#include "minisat/mtl/Vec.h"
#include "core/domain/assignment/Search.h"
#include "core/domain/assignment/FullSearch.h"
#include "core/proto/solve_config.pb.h"
#include "core/util/Registry.h"
#include "core/domain/assignment/TrackAssignments.h"

namespace core::sat::prop {

/**
 * @brief SAT Propagation interface.
 */
class Prop {
 public:
  /**
   * @details Callbacks types must be thread safe for asynchronous implementations.
   */
  // clang-format off
  typedef std::function<
      bool( // true iff should continue solving
        bool, // true iff there's been conflict
        Minisat::vec<Minisat::Lit> const& // assumptions passed to propagate
      )> prop_callback_t; //
  // clang-format on

 public:
  virtual ~Prop() = default;

  /**
   * @brief Parses cnf from .gz file with the specified path.
   * @param path the path with formula.
   */
  virtual void parse_cnf(std::filesystem::path const& path) = 0;

  /**
   * @brief Propagates a given list of assumptions.
   * @return true iff conflicts happened.
   * @param assumptions the assumptions to include.
   * @param propagated the propagated literals.
   * @return true if and only if conflict occurred.
   */
  [[nodiscard]] virtual bool propagate(
      Minisat::vec<Minisat::Lit> const& assumptions, Minisat::vec<Minisat::Lit>& propagated) = 0;

  /**
   * @brief Propagates a given list of assumptions.
   * @param assumptions assumptions to include.
   * @return true if and only if conflict occurred.
   */
  [[nodiscard]] virtual bool propagate(Minisat::vec<Minisat::Lit> const& assumptions);

  /**
   * @brief Propagates all assignments by the given iterator and calls callback respectively.
   * @param search search engine.
   * @param callback callback to be called on each solve.
   */
  virtual void prop_assignments(domain::USearch search, prop_callback_t const& callback);

  /// @todo: documentation
  virtual uint64_t prop_tree(Minisat::vec<Minisat::Lit> const& vars, uint32_t head_size) = 0;

  /**
   * @brief Returns the number of variables in formula.
   */
  [[nodiscard]] virtual uint32_t num_vars() const noexcept = 0;
};

using RProp = std::shared_ptr<Prop>;

using UProp = std::unique_ptr<Prop>;

DEFINE_REGISTRY(Prop, PropConfig, prop);

}  // namespace core::sat::prop

#endif  // EVOL_PROP_H