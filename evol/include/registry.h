#ifndef EA_REGISTRY_H
#define EA_REGISTRY_H

#include <map>
#include <string>

#include "evol/include/algorithm.h"
#include "evol/include/builder.h"

namespace ea::registry {

class Registry {
 public:
  Registry() = default;

  void register_algorithm(
      std::string const& name, builder::RBuilder<algorithm::IAlgorithm> builder);

  builder::RBuilder<algorithm::IAlgorithm> resolve_algorithm(std::string const& name);

  static Registry& get_registry();

 private:
  std::map<std::string, builder::RBuilder<algorithm::IAlgorithm>> map_;
};

template <typename Interface>
struct RegistryEntry {
  RegistryEntry(std::string const& name, builder::RBuilder<Interface> builder) {
    Registry::get_registry().register_algorithm(name, builder);
  }
};

}  // namespace ea::registry

#define REGISTER_ALGORITHM(IMPL)                                 \
  ::ea::registry::RegistryEntry<IAlgorithm> IMPL##RegistryEntry( \
      #IMPL, std::make_shared<IMPL##Builder>())

#endif  // EA_REGISTRY_H
