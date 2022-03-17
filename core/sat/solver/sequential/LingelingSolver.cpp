#include "LingelingSolver.h"

namespace core::sat::solver {

LingelingSolver::LingelingSolver() : _solver(lglinit()) {
  lglsetopt(_solver, "bca", 0);
  lglseterm(_solver, _term_callback, this);
}

LingelingSolver::~LingelingSolver() noexcept {
  lglrelease(_solver);
}

void LingelingSolver::load_problem(Problem const& problem) {
  for (auto const& clause : problem.clauses()) {
    for (int lit : clause) {
      lgladd(_solver, lit);
    }
    lgladd(_solver, 0);
  }
}

State LingelingSolver::solve(vec_lit_t const& assumptions) {
  // clang-format off
  IPS_TRACE_N(
    "LingelingSolver::solve", for (int i = 0; i < assumptions.size(); ++i) {
      lglassume(_solver, INT_LIT(assumptions[i]));
    }
    switch (lglsat(_solver)) {
      case LGL_SATISFIABLE:
        return SAT;
      case LGL_UNSATISFIABLE:
        return UNSAT;
    }
    return UNKNOWN;
  );
  // clang-format on
}

unsigned LingelingSolver::num_vars() const noexcept {
  return lglnvars(_solver);
}

void LingelingSolver::interrupt() {
  _should_stop = true;
}

void LingelingSolver::clear_interrupt() {
  _should_stop = false;
}

int LingelingSolver::_term_callback(void* solver) {
  return ((LingelingSolver*) solver)->_should_stop;
}

REGISTER_SIMPLE(Solver, LingelingSolver);

}  // namespace core::sat::solver