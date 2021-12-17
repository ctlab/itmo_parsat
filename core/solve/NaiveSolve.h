#ifndef ITMO_PARSAT_NAIVESOLVE_H
#define ITMO_PARSAT_NAIVESOLVE_H

#include "core/solve/Solve.h"
#include "core/util/SigHandler.h"
#include "core/util/Tracer.h"
#include "core/util/Logger.h"
#include "core/util/Generator.h"

namespace core {

class NaiveSolve : public Solve {
 public:
  NaiveSolve(NaiveSolveConfig const& config);

  [[nodiscard]] sat::State solve(std::filesystem::path const& input) override;

 private:
  NaiveSolveConfig _cfg;
};

}  // namespace core

#endif  // ITMO_PARSAT_NAIVESOLVE_H
