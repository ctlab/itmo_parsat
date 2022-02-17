#include "core/sat/prop/Prop.h"

namespace core::sat::prop {

bool Prop::propagate(Minisat::vec<Minisat::Lit> const& assumptions) {
  Minisat::vec<Minisat::Lit> props;
  return propagate(assumptions, props);
}

void Prop::prop_assignments(
    core::domain::USplittableSearch assignment_p, prop_callback_t const& callback) {
  domain::Search& assignment = *assignment_p;
  START_ASGN_TRACK(assignment_p->size());
  do {
    Minisat::vec<Minisat::Lit> const& assumptions = assignment();
    ASGN_TRACK(assumptions);
    if (!callback(propagate(assumptions), assumptions)) {
      BREAK_ASGN_TRACK;
      break;
    }
  } while (++assignment);
  END_ASGN_TRACK(false);
}

}  // namespace core::sat::prop