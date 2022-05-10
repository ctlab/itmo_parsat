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

class PortfolioSolve : public Solve {
 public:
  explicit PortfolioSolve(PortfolioSolveConfig config);

  [[nodiscard]] sat::State solve(sat::Problem const& problem) override;

  sat::sharing::SharingUnit sharing_unit() noexcept override;

  Mini::vec<Mini::lbool> get_model() const override;

 protected:
  void _interrupt_impl() override;

 private:
  PortfolioSolveConfig _cfg;
  std::vector<RSolve> _solves;
  std::optional<sat::sharing::Sharing> _sharing;
  Mini::vec<Mini::lbool> _model;
};

}  // namespace core::solve

#endif  // ITMO_PARSAT_WITHPAINLESSSOLVE_H
