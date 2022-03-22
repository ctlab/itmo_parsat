#ifndef ITMO_PARSAT_LINGELINGSOLVER_H
#define ITMO_PARSAT_LINGELINGSOLVER_H

#include "Solver.h"
#include "util/TimeTracer.h"
#include "core/sat/prop/SimpProp.h"

extern "C" {

#include "core/sat/native/lingeling/lglib.h"

}  // extern "C"

namespace core::sat::solver {

/**
 * @brief Lingeling-based Solver implementation
 */
class LingelingSolver : public Solver {
 public:
  explicit LingelingSolver();

  ~LingelingSolver() noexcept override;

  void load_problem(Problem const& problem) override;

  State solve(lit_vec_t const& assumptions) override;

  [[nodiscard]] unsigned num_vars() const noexcept override;

  void interrupt() override;

  void clear_interrupt() override;

 private:
  static int _term_callback(void* solver);

 private:
  LGL* _solver = nullptr;
  bool _should_stop = false;
};

}  // namespace core::sat::solver

#endif  // ITMO_PARSAT_LINGELINGSOLVER_H
