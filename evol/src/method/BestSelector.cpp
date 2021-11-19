#include "evol/include/method/BestSelector.h"

#include <algorithm>

namespace ea::selector {

void BestSelector::select(instance::Population& population) {
  auto it = std::max_element(population.begin(), population.end());
  std::swap(*it, population.front());
  population.resize(1);
}

}  // namespace ea::selector
