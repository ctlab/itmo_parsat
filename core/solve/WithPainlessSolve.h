#ifndef ITMO_PARSAT_WITHPAINLESSSOLVE_H
#define ITMO_PARSAT_WITHPAINLESSSOLVE_H

#include "core/sat/solver/sequential/Solver.h"
#include "core/sat/solver/sequential/PainlessSolver.h"
#include "core/solve/Solve.h"
#include "core/sat/sharing/Sharing.h"
#include "util/SigHandler.h"
#include "util/TimeTracer.h"
#include "util/Logger.h"
#include "util/Random.h"

namespace core::solve {

class WithPainlessSolve : public Solve {
 public:
  explicit WithPainlessSolve(WithPainlessSolveConfig config);

  [[nodiscard]] sat::State solve(sat::Problem const& problem) override;

  sat::sharing::SharingUnit sharing_unit() noexcept override;

 protected:
  void _interrupt_impl() override;

 private:
  WithPainlessSolveConfig _cfg;
  sat::solver::RSolver _painless_solver;
  RSolve _primary_solve;
  std::optional<sat::sharing::Sharing> _sharing;
};

}  // namespace core::solve

#endif  // ITMO_PARSAT_WITHPAINLESSSOLVE_H
