#include "evol/include/method/cross/TwoPointCrossover.h"

#include "evol/include/util/random.h"
#include "evol/include/util/Registry.h"

namespace ea::method {

void TwoPointCrossover::apply(instance::Instance& a, instance::Instance& b) {
  long num_vars = (long) instance::Instance::num_vars();
  long pos_1 = random::sample<long>(0, num_vars - 1);
  long pos_2 = random::sample<long>(0, num_vars - 1);
  if (pos_1 > pos_2) {
    std::swap(pos_1, pos_2);
  }

  // clang-format off
  std::swap_ranges(
    a.get_mask().begin() + pos_1, a.get_mask().begin() + pos_2,
    b.get_mask().begin() + pos_1
  );
  // clang-format on
}

REGISTER_SIMPLE(Crossover, TwoPointCrossover);

}  // namespace ea::method