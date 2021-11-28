#include "evol/include/util/Registry.h"

#include <glog/logging.h>

namespace {

template <typename K, typename V>
void check_register(std::map<K, V> const& map, K const& key, std::string const& interface) {
  assert(
      map.count(key) == size_t(0) &&
      bool("Trying to register implementation that is already registered."));
}

template <typename K, typename V>
void check_resolve(std::map<K, V> const& map, K const& key, std::string const& interface) {
  LOG(INFO) << "Resolving implementation " << key << " of " << interface;
  CHECK_GT(map.count(key), size_t(0)) << "Trying to resolve unregistered implementation '" << key
                                      << "' of interface " << interface << ".";
}

}  // namespace

namespace ea::registry {

Registry& Registry::instance() {
  static Registry reg_;
  return reg_;
}

#define IMPL_REGISTER_INTERFACE(INTERFACE, NAME)                                    \
  void Registry::register_##NAME(std::string const& name, NAME##_builder builder) { \
    check_register(NAME##_map_, name, #INTERFACE);                                  \
    NAME##_map_[name] = std::move(builder);                                         \
  }                                                                                 \
  std::shared_ptr<INTERFACE> Registry::resolve_##NAME(std::string const& name) {    \
    check_resolve(instance().NAME##_map_, name, #INTERFACE);                        \
    return instance().NAME##_map_[name]();                                          \
  }

IMPL_REGISTER_INTERFACE(::ea::algorithm::Algorithm, algorithm)
IMPL_REGISTER_INTERFACE(::ea::method::Selector, selector)
IMPL_REGISTER_INTERFACE(::ea::instance::Instance, instance)
IMPL_REGISTER_INTERFACE(::ea::limit::Limit, limit)
IMPL_REGISTER_INTERFACE(::ea::sat::Solver, solver)
IMPL_REGISTER_INTERFACE(::ea::method::Mutation, mutation)
IMPL_REGISTER_INTERFACE(::ea::method::Crossover, crossover)

}  // namespace ea::registry