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
 * @brief Mini:SimpSolver-based Solver implementation.
 */
class SimpProp : public Prop, public MinisatSimpBase {
 public:
  SimpProp() = default;

  void load_problem(Problem const& problem) override;

  [[nodiscard]] uint32_t num_vars() const noexcept override;

 protected:
  [[nodiscard]] bool _propagate(Mini::vec<Mini::Lit> const& assumptions) override;

  [[nodiscard]] bool _propagate(
      Mini::vec<Mini::Lit> const& assumptions, Mini::vec<Mini::Lit>& propagated) override;

  uint64_t _prop_tree(Mini::vec<Mini::Lit> const& vars, uint32_t head_size) override;
};

}  // namespace core::sat::prop

#endif  // EVOL_SIMPPROP_H
