#include "evol/include/method/cross/TwoPointCrossover.h"

#include "evol/include/util/random.h"
#include "evol/include/util/Registry.h"

namespace ea::method {

void TwoPointCrossover::apply(domain::Instance& a, domain::Instance& b) {
  long num_vars = (long) a.num_vars();
  long pos_1 = random::sample<long>(0, num_vars - 1);
  long pos_2 = random::sample<long>(0, num_vars - 1);
  if (pos_1 > pos_2) {
    std::swap(pos_1, pos_2);
  }
  auto& a_mask = a.get_vars().get_mask();
  auto& b_mask = b.get_vars().get_mask();
  std::swap_ranges(a_mask.begin() + pos_1, a_mask.begin() + pos_2, b_mask.begin() + pos_1);
}

REGISTER_SIMPLE(Crossover, TwoPointCrossover);

}  // namespace ea::method