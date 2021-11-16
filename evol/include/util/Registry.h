#ifndef EA_REGISTRY_H
#define EA_REGISTRY_H

#include <map>
#include <string>

#include "evol/include/algorithm/Algorithm.h"
#include "evol/include/method/Generator.h"
#include "evol/include/method/Selector.h"
#include "evol/include/util/Builder.h"

namespace ea::registry {

class Registry {
 public:
  Registry() = default;

 public:
  static Registry& instance();

#define DECLARE_INTERFACE(NAME, INTERFACE)                                             \
 public:                                                                               \
  void register_##NAME(std::string const& name, builder::RBuilder<INTERFACE> builder); \
  builder::RBuilder<INTERFACE> resolve_##NAME(std::string const& name);                \
 private:                                                                              \
  std::map<std::string, builder::RBuilder<INTERFACE>> NAME##_map_{};

  DECLARE_INTERFACE(algorithm, ea::algorithm::Algorithm);
  DECLARE_INTERFACE(instance, ea::instance::Instance);
  DECLARE_INTERFACE(generator, ea::generator::Generator);
  DECLARE_INTERFACE(selector, ea::selector::Selector);
};

template <typename Interface>
struct RegistryEntry;

#define DEFINE_ENTRY_TYPE(INTERFACE, REGISTRY_METHOD)                                       \
  template <>                                                                               \
  struct RegistryEntry<INTERFACE> {                                                         \
    RegistryEntry(std::string const& name, builder::RBuilder<INTERFACE> builder) noexcept { \
      Registry::instance().REGISTRY_METHOD(name, builder);                                  \
    }                                                                                       \
  }

DEFINE_ENTRY_TYPE(ea::algorithm::Algorithm, register_algorithm);
DEFINE_ENTRY_TYPE(ea::instance::Instance, register_instance);
DEFINE_ENTRY_TYPE(ea::generator::Generator, register_generator);
DEFINE_ENTRY_TYPE(ea::selector::Selector, register_selector);

}  // namespace ea::registry

#define REGISTER(INTERFACE, IMPL)                               \
  ::ea::registry::RegistryEntry<INTERFACE> IMPL##RegistryEntry( \
      #IMPL, std::make_shared<::ea::builder::IMPL##Builder>())

#endif  // EA_REGISTRY_H
