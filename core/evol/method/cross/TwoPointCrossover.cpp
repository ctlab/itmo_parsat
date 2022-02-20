#include "core/evol/method/cross/TwoPointCrossover.h"

#include "util/Random.h"

namespace ea::method {

void TwoPointCrossover::apply(instance::Instance& a, instance::Instance& b) {
  long num_vars = (long) a.num_vars();
  long pos_1 = util::random::sample<long>(0, num_vars - 1);
  long pos_2 = util::random::sample<long>(0, num_vars - 1);
  if (pos_1 > pos_2) {
    std::swap(pos_1, pos_2);
  }
  auto& a_mask = a.get_vars().get_mask();
  auto& b_mask = b.get_vars().get_mask();
  std::swap_ranges(a_mask.begin() + pos_1, a_mask.begin() + pos_2, b_mask.begin() + pos_1);
}

REGISTER_SIMPLE(Crossover, TwoPointCrossover);

}  // namespace ea::method