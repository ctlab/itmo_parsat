#include "evol/include/method/BestSelector.h"

#include <algorithm>

#include "evol/include/util/Registry.h"

namespace ea::selector {

void BestSelector::select(instance::Population& population) {
  auto it = std::max_element(population.begin(), population.end(), [](auto& a, auto& b) {
    return *a < *b;
  });
  std::swap(*it, population.front());
  population.resize(1);
}

REGISTER_SIMPLE(Selector, BestSelector);

}  // namespace ea::selector
