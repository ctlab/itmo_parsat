#ifndef ITMO_PARSAT_SBSSOLVE_H
#define ITMO_PARSAT_SBSSOLVE_H

#include <cmath>
#include <boost/timer/progress_display.hpp>

#include "core/evol/algorithm/Algorithm.h"
#include "core/solve/Solve.h"
#include "core/util/SigHandler.h"
#include "core/util/Tracer.h"
#include "core/util/Logger.h"

namespace core {

class SBSSolve : public Solve {
 public:
  SBSSolve(SBSSolveConfig const& config);

  [[nodiscard]] sat::State solve(std::filesystem::path const& input) override;

 private:
  SBSSolveConfig _cfg;
};

}  // namespace core

#endif  // ITMO_PARSAT_SBSSOLVE_H
