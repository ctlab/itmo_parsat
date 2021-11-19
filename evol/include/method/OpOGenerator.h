#ifndef EVOL_OPOGENERATOR_H
#define EVOL_OPOGENERATOR_H

#include "evol/include/method/Generator.h"
#include "evol/include/util/Registry.h"

namespace ea::generator {

class OpOGenerator : public Generator {
 public:
  void generate(instance::Population& population) override;
};

}  // namespace ea::generator

#endif  // EVOL_OPOGENERATOR_H
