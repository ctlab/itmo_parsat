#ifndef EVOL_SIMPPROP_H
#define EVOL_SIMPPROP_H

#include <filesystem>

#include "core/sat/prop/Prop.h"
#include "util/Logger.h"
#include "util/TimeTracer.h"

#include "core/sat/native/mini/minisat/minisat/core/Dimacs.h"
#include "core/sat/native/mini/minisat/minisat/simp/SimpSolver.h"

namespace core::sat::prop {

SIMP_BASE(Minisat);

/**
 * @brief Minisat-based propagation engine implementation.
 * @note Does not support multi-threaded use (use ParProp).
 */
class SimpProp : public Prop, public MinisatSimpBase {
 public:
  SimpProp() = default;

  void load_problem(Problem const& problem) override;

  [[nodiscard]] uint32_t num_vars() const noexcept override;

  [[nodiscard]] bool propagate(
      lit_vec_t const& assumptions, lit_vec_t& propagated) override;

  [[nodiscard]] bool propagate(lit_vec_t const& assumptions) override;

  uint64_t prop_search(
      search::USearch search, prop_callback_t const& callback = {}) override;

  uint64_t prop_search(
      lit_vec_t const& base_assumption, search::USearch search,
      prop_callback_t const& callback = {}) override;

  uint64_t prop_tree(lit_vec_t const& vars, uint32_t head_size) override;
};

}  // namespace core::sat::prop

#endif  // EVOL_SIMPPROP_H
