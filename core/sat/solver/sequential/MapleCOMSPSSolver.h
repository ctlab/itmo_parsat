#ifndef EVOL_SIMPSOLVER_H
#define EVOL_SIMPSOLVER_H

#include <filesystem>

#include "Solver.h"
#include "util/Logger.h"
#include "util/TimeTracer.h"
#include "core/sat/SimpBase.h"

#include "core/sat/native/mini/mapleCOMSPS/mapleCOMSPS/simp/SimpSolver.h"
#include "core/sat/native/mini/mapleCOMSPS/mapleCOMSPS/core/Dimacs.h"
#include "core/sat/native/mini/mapleCOMSPS/mapleCOMSPS/core/SolverTypes.h"

namespace core::sat::solver {

SIMP_BASE(MapleCOMSPS);

/**
 * @brief MS_NS:MapleCOMSPSSolver-based Solver implementation.
 */
class MapleCOMSPSSolver : public Solver, public MapleCOMSPSSimpBase {
 public:
  MapleCOMSPSSolver() = default;

  void load_problem(Problem const& problem) override;

  State solve(lit_vec_t const& assumptions) override;

  [[nodiscard]] unsigned num_vars() const noexcept override;

  void interrupt() override;

  void clear_interrupt() override;
};

}  // namespace core::sat::solver

#endif  // EVOL_SIMPSOLVER_H
