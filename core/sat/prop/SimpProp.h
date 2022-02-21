#ifndef EVOL_SIMPPROP_H
#define EVOL_SIMPPROP_H

#include <filesystem>

#include "core/sat/prop/Prop.h"
#include "util/Logger.h"
#include "util/TimeTracer.h"
#include "core/sat/SimpBase.h"
#include "minisat/simp/SimpSolver.h"

namespace core::sat::prop {

/**
 * @brief Minisat:SimpSolver-based Solver implementation.
 */
class SimpProp : public Prop, public SimpBase {
 public:
  SimpProp() = default;

  void load_problem(Problem const& problem) override;

  [[nodiscard]] uint32_t num_vars() const noexcept override;

  [[nodiscard]] bool propagate(
      Minisat::vec<Minisat::Lit> const& assumptions,
      Minisat::vec<Minisat::Lit>& propagated) override;

  [[nodiscard]] bool propagate(Minisat::vec<Minisat::Lit> const& assumptions) override;

  uint64_t prop_tree(Minisat::vec<Minisat::Lit> const& vars, uint32_t head_size) override;
};

}  // namespace core::sat::prop

#endif  // EVOL_SIMPPROP_H
