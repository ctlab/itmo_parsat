#include "core/sat/prop/SimpProp.h"

namespace core::sat::prop {

void SimpProp::load_problem(Problem const& problem) {
  MinisatSimpBase::load_problem(problem);
}

uint32_t SimpProp::num_vars() const noexcept {
  return MinisatSimpBase::num_vars();
}

bool SimpProp::propagate(lit_vec_t const& assumptions, lit_vec_t& propagated) {
  return !prop_check(assumptions, propagated, 0);
}

bool SimpProp::propagate(lit_vec_t const& assumptions) {
  return !prop_check(assumptions, 0);
}

uint64_t SimpProp::prop_tree(lit_vec_t const& vars, uint32_t head_size) {
  return prop_check_subtree(vars, head_size);
}

REGISTER_SIMPLE(Prop, SimpProp);

}  // namespace core::sat::prop
