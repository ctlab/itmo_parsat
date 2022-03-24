#ifndef ITMO_PARSAT_SIMPBASE_H
#define ITMO_PARSAT_SIMPBASE_H

#include <filesystem>

#include "core/types.h"
#include "core/sat/Problem.h"
#include "util/GzFile.h"
#include "util/TimeTracer.h"

#define MINI_LIT(lit) \
  lit > 0 ? Mini::mkLit(lit - 1, false) : Mini::mkLit((-lit) - 1, true)
#define INT_LIT(lit) \
  Mini::sign(lit) ? -(Mini::var(lit) + 1) : (Mini::var(lit) + 1)

namespace core::sat {

#define SIMP_BASE(MS_NS)                                  \
  class MS_NS##SimpBase : public MS_NS::SimpSolver {      \
   public:                                                \
    MS_NS##SimpBase() = default;                          \
                                                          \
    [[nodiscard]] uint32_t num_vars() const noexcept {    \
      return nVars();                                     \
    }                                                     \
                                                          \
    bool load_problem(Problem const& problem) {           \
      parsing = true;                                     \
      MS_NS::SimpSolver::loadClauses(problem.clauses());  \
      parsing = false;                                    \
      return true;                                        \
    }                                                     \
                                                          \
   protected:                                             \
    bool _propagate_confl(lit_vec_t const& assumptions) { \
      return !prop_check(assumptions, (int) 0);           \
    }                                                     \
  }

}  // namespace core::sat

#endif  // ITMO_PARSAT_SIMPBASE_H
