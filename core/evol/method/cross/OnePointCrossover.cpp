#include "core/evol/method/cross/OnePointCrossover.h"

#include "core/util/random.h"

namespace ea::method {

void OnePointCrossover::apply(instance::Instance& a, instance::Instance& b) {
  long num_vars = (long) a.num_vars();
  long pos = core::random::sample<long>(0, num_vars - 1);
  auto& a_mask = a.get_vars().get_mask();
  auto& b_mask = b.get_vars().get_mask();
  if (pos < num_vars - pos) {
    std::swap_ranges(a_mask.begin(), a_mask.begin() + pos, b_mask.begin());
  } else {
    std::swap_ranges(a_mask.begin() + pos, a_mask.end(), b_mask.begin() + pos);
  }
}

REGISTER_SIMPLE(Crossover, OnePointCrossover, CrossoverConfig);

}  // namespace ea::method