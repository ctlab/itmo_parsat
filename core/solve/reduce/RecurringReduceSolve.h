#ifndef ITMO_PARSAT_RECURRINGREDUCESOLVE_H
#define ITMO_PARSAT_RECURRINGREDUCESOLVE_H

#include <stack>

#include "core/proto/solve_config.pb.h"
#include "core/solve/reduce/ReduceSolve.h"
#include "core/solve/rbs/RBSearch.h"
#include "util/visit.h"
#include "util/stream.h"
#include "util/Logger.h"

namespace core::solve {

/**
 * @brief This implementation acts as follows:
 *  - Given base assumptions, reduce the current task to a set of tasks.
 *  - In the resulting set of tasks, solve ones that can be solved fast enough.
 *  - The rest of problems are put into stack and processed in the same manner.
 */
class RecurringReduceSolve : public ReduceSolve {
 private:
  using filter_r = std::variant<sat::State, std::vector<Mini::vec<Mini::Lit>>>;

 public:
  explicit RecurringReduceSolve(RecurringReduceSolveConfig config);

 protected:
  sat::State _solve_impl(
      ea::preprocess::RPreprocess const& preprocess) override;

  void _interrupt_impl() override;

 private:
  filter_r _filter_fast(
      std::vector<lit_vec_t> const& assumptions, clock_t::duration time_limit);

 private:
  RRBSearch _rb_search;
  RecurringReduceSolveConfig _cfg;
  lit_vec_t _cur_base_assumption;
};

}  // namespace core::solve

#endif  // ITMO_PARSAT_RECURRINGREDUCESOLVE_H
