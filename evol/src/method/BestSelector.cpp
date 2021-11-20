#include "evol/include/method/BestSelector.h"
#include "evol/include/util/Registry.h"

#include <algorithm>

namespace ea::selector {

void BestSelector::select(instance::Population& population) {
  auto it = std::max_element(population.begin(), population.end());
  std::swap(*it, population.front());
  population.resize(1);
}

REGISTER_SIMPLE(Selector, BestSelector);

}  // namespace ea::selector
