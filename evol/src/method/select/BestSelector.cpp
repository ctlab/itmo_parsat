#include "evol/include/method/select/BestSelector.h"

#include <algorithm>

#include "evol/include/util/Registry.h"

namespace ea::method {

void BestSelector::select(domain::Population& population, size_t size) {
  if (population.size() > size) {
    std::sort(population.begin(), population.end(), [](auto& a, auto& b) { return *a < *b; });
    population.resize(size);
  }
}

REGISTER_SIMPLE(Selector, BestSelector);

}  // namespace ea::method