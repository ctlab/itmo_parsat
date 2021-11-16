#include "evol/include/util/Registry.h"

#include <glog/logging.h>

#include <utility>

#include "evol/include/algorithm/Algorithm.h"

namespace {

template <typename K, typename V>
void check_register(std::map<K, V> const& map, K const& key, char const* name) {
  CHECK(map.count(key) == 0) << "Trying to register an existing " << name
                             << " implementation: " << key;
}

template <typename K, typename V>
void check_resolve(std::map<K, V> const& map, K const& key, char const* name) {
  CHECK(map.count(key) > 0) << "Trying to resolve non-existing " << name
                            << " implementation: " << key;
}

}  // namespace

namespace ea::registry {

#define DEFINE_REGISTRY(NAME, INTERFACE)                                                          \
  void Registry::register_##NAME(std::string const& name, builder::RBuilder<INTERFACE> builder) { \
    check_register(NAME##_map_, name, #INTERFACE);                                                \
    NAME##_map_[name] = std::move(builder);                                                       \
  }                                                                                               \
  builder::RBuilder<INTERFACE> Registry::resolve_##NAME(std::string const& name) {                \
    check_resolve(NAME##_map_, name, #INTERFACE);                                                 \
    return NAME##_map_[name];                                                                     \
  }

DEFINE_REGISTRY(algorithm, ea::algorithm::Algorithm);
DEFINE_REGISTRY(instance, ea::instance::Instance);
DEFINE_REGISTRY(generator, ea::generator::Generator);
DEFINE_REGISTRY(selector, ea::selector::Selector);

Registry& Registry::instance() {
  static Registry registry{};
  return registry;
}

}  // namespace ea::registry
