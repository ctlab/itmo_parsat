#include "evol/include/method/cross/OnePointCrossover.h"

#include "evol/include/util/random.h"
#include "evol/include/util/Registry.h"

namespace ea::method {

void OnePointCrossover::apply(instance::RInstance& a, instance::RInstance& b) {
  long num_vars = (long) instance::Instance::num_vars();
  long pos = random::sample<long>(0, num_vars - 1);

  if (pos < num_vars - pos) {
    std::swap_ranges(
        a->get_variables().begin(), a->get_variables().begin() + pos, b->get_variables().begin());
  } else {
    // clang-format off
    std::swap_ranges(
        a->get_variables().begin() + pos, a->get_variables().end(),
        b->get_variables().begin()) + pos;
    // clang-format on
  }

  a->recalc_vars();
  b->recalc_vars();
}

REGISTER_SIMPLE(Crossover, OnePointCrossover);

}  // namespace ea::method