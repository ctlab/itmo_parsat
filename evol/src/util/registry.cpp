#include "evol/include/registry.h"

#include <glog/logging.h>

#include "evol/include/algorithm.h"

namespace ea::registry {

void Registry::register_algorithm(
    std::string const& name, builder::RBuilder<algorithm::IAlgorithm> builder) {
  CHECK(map_.count(name) == 0) << "Trying to register an existing "
                                  "algorithm::IAlgorithm implementation.";
  map_[name] = builder;
}

builder::RBuilder<algorithm::IAlgorithm> Registry::resolve_algorithm(std::string const& name) {
  CHECK(map_.count(name) > 0) << "algorithm::IAlgorithm resolve failed: no such implementation: "
                              << name;
  return map_[name];
}

Registry& Registry::get_registry() {
  static Registry registry{};
  return registry;
}

}  // namespace ea::registry

