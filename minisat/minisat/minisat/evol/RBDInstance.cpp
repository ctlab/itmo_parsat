#include "RBDInstance.h"

#include "util/random.h"

namespace ea {

instance::Instance* RBDInstance::clone() const {
  return new RBDInstance(*this);
}

double RBDInstance::fitness() const {
  /* TODO */
  /* generate number of samples */
  /* evaluate \xi_i for each of them */
  /* then evaluate the fitness */
  return 0.;
}

instance::RInstance RBDInstance::crossover(instance::RInstance const& a) const {
  /* TODO: not needed for (1 + 1)-ea */
}

instance::RInstance RBDInstance::random() {

}

void RBDInstance::mutate() {

}

}  // namespace ea