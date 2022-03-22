#include "core/evol/method/select/BestSelector.h"

#include <algorithm>

namespace ea::method {

void BestSelector::select(instance::Population& population, uint32_t size) {
  if (population.size() > size) {
    std::sort(population.begin(), population.end(), [](auto& a, auto& b) {
      return *a < *b;
    });
    population.resize(size);
  }
}

REGISTER_SIMPLE(Selector, BestSelector);

}  // namespace ea::method
