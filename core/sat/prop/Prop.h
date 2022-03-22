#ifndef EVOL_PROP_H
#define EVOL_PROP_H

#include <functional>
#include <filesystem>
#include <memory>
#include <atomic>

#include "core/types.h"
#include "core/sat/SimpBase.h"
#include "core/domain/assignment/Search.h"
#include "core/domain/assignment/FullSearch.h"
#include "core/proto/solve_config.pb.h"
#include "core/sat/Problem.h"
#include "util/Registry.h"
#include "util/mini.h"

namespace core::sat::prop {

/**
 * @brief SAT Propagation interface.
 */
class Prop {
 public:
  /**
   * @details Callbacks types must be thread safe for asynchronous
   * implementations.
   */
  // clang-format off
  typedef std::function<
      bool( // true iff should continue solving
        bool, // true iff there's been conflict
        lit_vec_t const& // assumptions passed to propagate
      )> prop_callback_t; //
  // clang-format on

 public:
  virtual ~Prop() = default;

  /**
   * @brief Parses cnf from .gz file with the specified path.
   * @param path the path with formula.
   */
  virtual void load_problem(Problem const& problem) = 0;

  /**
   * @brief Propagates a given list of assumptions.
   * @return true iff conflicts happened.
   * @param assumptions the assumptions to include.
   * @param propagated the propagated literals.
   * @return true if and only if conflict occurred.
   */
  [[nodiscard]] bool propagate(
      lit_vec_t const& assumptions, lit_vec_t& propagated);

  /**
   * @brief Propagates a given list of assumptions.
   * @param assumptions assumptions to include.
   * @return true if and only if conflict occurred.
   */
  [[nodiscard]] bool propagate(lit_vec_t const& assumptions);

  /**
   * @brief Propagates all assignments by the given iterator and calls callback
   * respectively.
   * @param search search engine.
   * @param callback callback to be called on each solve.
   */
  void prop_assignments(
      domain::USearch search, prop_callback_t const& callback);

  void prop_assignments(
      lit_vec_t const& base_assumption, domain::USearch search,
      prop_callback_t const& callback);

  /// @todo: documentation
  uint64_t prop_tree(lit_vec_t const& vars, uint32_t head_size);

  /**
   * @brief Returns the number of variables in formula.
   */
  [[nodiscard]] virtual uint32_t num_vars() const noexcept = 0;

  static void sequential_propagate(
      Prop& prop, domain::USearch search, prop_callback_t const& callback);

  static void sequential_propagate(
      lit_vec_t const& base_assumption, Prop& prop, domain::USearch search,
      prop_callback_t const& callback);

 protected:
  [[nodiscard]] virtual uint64_t _prop_tree(
      lit_vec_t const& vars, uint32_t head_size) = 0;

  [[nodiscard]] virtual bool _propagate(
      lit_vec_t const& assumptions, lit_vec_t& propagated) = 0;

  [[nodiscard]] virtual bool _propagate(lit_vec_t const& assumptions);

  virtual void _prop_assignments(
      domain::USearch search, prop_callback_t const& callback);

  virtual void _prop_assignments(
      lit_vec_t const& base_assumption, domain::USearch search,
      prop_callback_t const& callback);

 private:
  std::mutex _m;
};

MAKE_REFS(Prop);

DEFINE_REGISTRY(Prop, PropConfig, prop);

}  // namespace core::sat::prop

#endif  // EVOL_PROP_H