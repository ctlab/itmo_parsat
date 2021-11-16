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

namespace ea::builder {

class OpOGeneratorBuilder : public Builder<ea::generator::Generator> {
 public:
  OpOGeneratorBuilder() = default;

  ea::generator::Generator* build() override;

  char const** get_params() override;
};

}  // namespace ea::builder

REGISTER(ea::generator::Generator, OpOGenerator);

#endif  // EVOL_OPOGENERATOR_H
