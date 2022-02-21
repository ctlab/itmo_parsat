#ifndef ITMO_PARSAT_LINGELINGSOLVER_H
#define ITMO_PARSAT_LINGELINGSOLVER_H

#include "core/sat/solver/Solver.h"
#include "util/TimeTracer.h"
#include "core/sat/prop/SimpProp.h"

extern "C" {

#include "core/sat/native/lingeling/lglib.h"

}  // extern "C"

namespace core::sat::solver {

class LingelingSolver : public Solver {
 public:
  explicit LingelingSolver(/* LingelingSolverConfig */);

  ~LingelingSolver() noexcept override;

  void load_problem(Problem const& problem) override;

  State solve(Minisat::vec<Minisat::Lit> const& assumptions) override;

  [[nodiscard]] unsigned num_vars() const noexcept override;

  [[nodiscard]] bool propagate_confl(Minisat::vec<Minisat::Lit> const& assumptions) override;

 private:
  static int _term_callback(void* solver);

  void _do_interrupt() override;

  void _do_clear_interrupt() override;

 private:
  LGL* _solver = nullptr;
  bool _should_stop = false;
};

}  // namespace core::sat::solver

#endif  // ITMO_PARSAT_LINGELINGSOLVER_H
