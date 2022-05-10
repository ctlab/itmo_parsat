#ifndef CORE_LMAPLESOLVER_H
#define CORE_LMAPLESOLVER_H

#include <filesystem>

#include "Solver.h"
#include "util/Logger.h"
#include "util/TimeTracer.h"
#include "core/sat/SimpBase.h"
#include "core/sat/native/painless/painless-src/solvers/LMapleSolver.h"

namespace core::sat::solver {

/**
 * @brief Painless-adapted MapleCOMSPSSolver solver implementation.
 */
class LMapleSolver : public Solver, public painless::LMapleSolver {
 public:
  explicit LMapleSolver(LMapleSolverConfig const& config);

  State solve(lit_vec_t const& assumptions) override;

  void load_problem(Problem const& problem) override;

  [[nodiscard]] unsigned num_vars() const noexcept override;

  void interrupt() override;

  void clear_interrupt() override;

  sharing::SharingUnit sharing_unit() noexcept override;

  Mini::vec<Mini::lbool> get_model() const noexcept override;
};

}  // namespace core::sat::solver

#endif  // CORE_LMAPLESOLVER_H
