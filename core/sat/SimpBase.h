#ifndef ITMO_PARSAT_SIMPBASE_H
#define ITMO_PARSAT_SIMPBASE_H

#include <filesystem>

#include "minisat/simp/SimpSolver.h"
#include "minisat/core/Dimacs.h"
#include "core/proto/solve_config.pb.h"
#include "core/sat/Problem.h"
#include "util/Generator.h"
#include "util/GzFile.h"
#include "util/Logger.h"
#include "util/TimeTracer.h"

namespace core::sat {

class SimpBase : public Minisat::SimpSolver {
 public:
  SimpBase() = default;

  explicit SimpBase(SimpSolverConfig const& config);

  [[nodiscard]] uint32_t num_vars() const noexcept;

  /// @brief returns false if the formula has been solved during initialization (UNSAT)
  bool load_problem(Problem const& problem);

 protected:
  bool _propagate_confl(Minisat::vec<Minisat::Lit> const& assumptions);
};

}  // namespace core::sat

#endif  // ITMO_PARSAT_SIMPBASE_H
