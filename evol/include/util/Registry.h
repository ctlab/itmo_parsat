#ifndef EA_REGISTRY_H
#define EA_REGISTRY_H

#include <functional>
#include <map>
#include <memory>

#include "evol/include/algorithm/Algorithm.h"
#include "evol/include/config/Configuration.h"
#include "evol/include/domain/Instance.h"
#include "evol/include/limit/Limit.h"
#include "evol/include/method/select/Selector.h"
#include "evol/include/sat/Solver.h"
#include "evol/include/method/mutate/Mutation.h"
#include "evol/include/method/cross/Crossover.h"

namespace ea::registry {

class Registry {
 public:
  Registry() = default;

  static Registry& instance();

#define REGISTER_INTERFACE(INTERFACE, NAME)                                  \
 public:                                                                     \
  using NAME##_builder = std::function<std::shared_ptr<INTERFACE>()>;        \
  void register_##NAME(std::string const& name, NAME##_builder builder);     \
  static std::shared_ptr<INTERFACE> resolve_##NAME(std::string const& name); \
                                                                             \
 private:                                                                    \
  std::map<std::string, NAME##_builder> NAME##_map_

  REGISTER_INTERFACE(::ea::algorithm::Algorithm, algorithm);
  REGISTER_INTERFACE(::ea::method::Selector, selector);
  REGISTER_INTERFACE(::ea::domain::Instance, instance);
  REGISTER_INTERFACE(::ea::limit::Limit, limit);
  REGISTER_INTERFACE(::ea::sat::Solver, solver);
  REGISTER_INTERFACE(::ea::method::Mutation, mutation);
  REGISTER_INTERFACE(::ea::method::Crossover, crossover);
};

template <typename Interface>
struct RegistryEntry;

#define DEFINE_ENTRY_TYPE(INTERFACE, NAME)                                                        \
  template <>                                                                                     \
  struct RegistryEntry<INTERFACE> {                                                               \
    RegistryEntry(std::string const& name, std::function<std::shared_ptr<INTERFACE>()> builder) { \
      Registry::instance().register_##NAME(name, std::move(builder));                             \
    }                                                                                             \
  }

DEFINE_ENTRY_TYPE(::ea::algorithm::Algorithm, algorithm);
DEFINE_ENTRY_TYPE(::ea::method::Selector, selector);
DEFINE_ENTRY_TYPE(::ea::domain::Instance, instance);
DEFINE_ENTRY_TYPE(::ea::limit::Limit, limit);
DEFINE_ENTRY_TYPE(::ea::sat::Solver, solver);
DEFINE_ENTRY_TYPE(::ea::method::Mutation, mutation);
DEFINE_ENTRY_TYPE(::ea::method::Crossover, crossover);

}  // namespace ea::registry

#define REGISTER_PROTO(INTERFACE, IMPL, CONFIG_NAME)                                \
  std::shared_ptr<INTERFACE> create##IMPL() {                                       \
    return std::make_shared<IMPL>(                                                  \
        ::ea::config::Configuration::instance().get_global_config().CONFIG_NAME()); \
  }                                                                                 \
  ea::registry::RegistryEntry<INTERFACE> regentry_##IMPL(#IMPL, []() { return create##IMPL(); })

#define REGISTER_SIMPLE(INTERFACE, IMPL)      \
  std::shared_ptr<INTERFACE> create##IMPL() { \
    return std::make_shared<IMPL>();          \
  }                                           \
  ea::registry::RegistryEntry<INTERFACE> regentry_##IMPL(#IMPL, []() { return create##IMPL(); })

#endif  // EA_REGISTRY_H
