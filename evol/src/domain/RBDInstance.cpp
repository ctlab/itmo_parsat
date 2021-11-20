#include "evol/include/domain/RBDInstance.h"
#include "evol/include/util/Registry.h"

#include <glog/logging.h>

namespace ea::instance {

Instance* RBDInstance::clone() const {
  return new RBDInstance(*this);
}

double RBDInstance::fitness() const {
  /* TODO */
  sleep(1);
  return 0.5;
}

RInstance RBDInstance::crossover(RInstance const& other) const {
  /* TODO */
  LOG(FATAL) << "Crossover is not yet implemented for RBDInstance.";
  return nullptr;
}

void RBDInstance::mutate() {
  /* TODO */
  sleep(1);
}

REGISTER_SIMPLE(Instance, RBDInstance);

}  // namespace ea::instance