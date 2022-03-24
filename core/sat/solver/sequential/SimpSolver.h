#ifndef EVOL_SIMPSOLVER_H
#define EVOL_SIMPSOLVER_H

#include <filesystem>

#include "Solver.h"
#include "util/Logger.h"
#include "util/TimeTracer.h"
#include "core/sat/SimpBase.h"

#include "core/sat/native/mini/minisat/minisat/simp/SimpSolver.h"
#include "core/sat/native/mini/minisat/minisat/core/Dimacs.h"
#include "core/sat/native/mini/utils/ParseUtils.h"

namespace core::sat::solver {

SIMP_BASE(Minisat);

/**
 * @brief MS_NS:SimpSolver-based Solver implementation.
 */
class SimpSolver : public Solver, public MinisatSimpBase {
 public:
  SimpSolver() = default;

  State solve(lit_vec_t const& assumptions) override;

  void load_problem(Problem const& problem) override;

  [[nodiscard]] unsigned num_vars() const noexcept override;

  void interrupt() override;

  void clear_interrupt() override;
};

}  // namespace core::sat::solver

#endif  // EVOL_SIMPSOLVER_H
