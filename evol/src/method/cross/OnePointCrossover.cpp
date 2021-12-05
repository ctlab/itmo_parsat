#include "evol/include/method/cross/OnePointCrossover.h"

#include "evol/include/util/random.h"
#include "evol/include/util/Registry.h"

namespace ea::method {

void OnePointCrossover::apply(instance::Instance& a, instance::Instance& b) {
  long num_vars = (long) instance::Instance::num_vars();
  long pos = random::sample<long>(0, num_vars - 1);

  // clang-format off
  if (pos < num_vars - pos) {
    std::swap_ranges(
      a.get_mask().begin(), a.get_mask().begin() + pos,
      b.get_mask().begin()
    );
  } else {
    std::swap_ranges(
      a.get_mask().begin() + pos, a.get_mask().end(),
      b.get_mask().begin() + pos
    );
  }
  // clang-format on
}

REGISTER_SIMPLE(Crossover, OnePointCrossover);

}  // namespace ea::method