#include "core/sat/prop/SimpProp.h"

namespace core::sat::prop {

bool SimpProp::propagate(
    Mini::vec<Mini::Lit> const& assumptions, Mini::vec<Mini::Lit>& propagated) {
  clearInterrupt();
  return IPS_TRACE_N_V("SimpProp::propagate_prop", !prop_check(assumptions, propagated, 0));
}

bool SimpProp::propagate(Mini::vec<Mini::Lit> const& assumptions) {
  clearInterrupt();
  return IPS_TRACE_N_V("SimpProp::propagate_confl", !prop_check(assumptions, 0));
}

uint64_t SimpProp::prop_tree(Mini::vec<Mini::Lit> const& vars, uint32_t head_size) {
  clearInterrupt();
  return IPS_TRACE_N_V("SimpProp::prop_tree", prop_check_subtree(vars, head_size));
}

void SimpProp::load_problem(Problem const& problem) {
  MinisatSimpBase::load_problem(problem);
}

uint32_t SimpProp::num_vars() const noexcept {
  return MinisatSimpBase::num_vars();
}

REGISTER_SIMPLE(Prop, SimpProp);

}  // namespace core::sat::prop
