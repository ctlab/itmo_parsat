#include "evol/include/method/BestSelector.h"

#include <algorithm>

#include "evol/include/util/Registry.h"

namespace ea::selector {

void BestSelector::select(instance::Population& population) {
  auto it = std::max_element(population.begin(), population.end());
  std::swap(*it, population.front());
  population.resize(1);
}

}  // namespace ea::selector

namespace ea::builder {

selector::Selector* BestSelectorBuilder::build() {
  return new selector::BestSelector;
}

char const** BestSelectorBuilder::get_params() {
  static char const* params_[] = {};
  return params_;
}

}  // namespace ea::builder

/* Marked noexcept explicitly */
REGISTER(ea::selector::Selector, BestSelector);
