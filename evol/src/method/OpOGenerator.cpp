#include "evol/include/method/OpOGenerator.h"

#include <glog/logging.h>

namespace ea::generator {

void OpOGenerator::generate(instance::Population& population) {
  CHECK(population.size() == 1)
      << "OpOGenerator: Invalid state: population size is not equal to 1, but is "
      << population.size();
  instance::RInstance const& parent = population.front();
  instance::RInstance child(parent->clone());
  child->mutate();
  population.push_back(child);
}

}  // namespace ea::generator

namespace ea::builder {

ea::generator::Generator* OpOGeneratorBuilder::build() {
  return new ea::generator::OpOGenerator();
}

char const** OpOGeneratorBuilder::get_params() {
  static char const* params_[] = {};
  return params_;
}

}  // namespace ea::builder