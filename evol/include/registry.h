#ifndef EA_REGISTRY_H
#define EA_REGISTRY_H

#include <string>

#include "evol/include/algorithm.h"
#include "evol/include/builder.h"

namespace ea::registry {

void register_algorithm(
    std::string const& name, builder::RBuilder<algorithm::IAlgorithm> builder);

builder::RBuilder<algorithm::IAlgorithm> resolve_algorithm(
    std::string const& name);

template <typename Interface>
struct RegistryEntry {
  RegistryEntry(std::string const& name, builder::RBuilder<Interface> builder) {
    register_algorithm(name, builder);
  }
};

}  // namespace ea::registry

#define REGISTER_ALGORITHM(IMPL)                                 \
  ::ea::registry::RegistryEntry<IAlgorithm> IMPL##RegistryEntry( \
      #IMPL, std::make_shared<IMPL##Builder>())

#endif  // EA_REGISTRY_H
