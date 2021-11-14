#include "evol/include/registry.h"

#include <glog/logging.h>

#include <map>

#include "evol/include/algorithm.h"

namespace ea::internal_::registry {

class Registry {
 public:
  Registry() = default;

  void register_algorithm(
      std::string const& name,
      builder::RBuilder<algorithm::IAlgorithm> builder) {
    CHECK(map_.count(name) == 0) << "Trying to register an existing "
                                    "algorithm::IAlgorithm implementation.";
    map_[name] = builder;
  }

  builder::RBuilder<algorithm::IAlgorithm> resolve_algorithm(
      std::string const& name) {
    CHECK(map_.count(name) > 0)
        << "algorithm::IAlgorithm resolve failed: no such implementation: "
        << name;
    return map_[name];
  }

 private:
  std::map<std::string, builder::RBuilder<algorithm::IAlgorithm>> map_;
};

Registry registry_{};

}  // namespace ea::internal_::registry

namespace ea::registry {

void register_algorithm(
    std::string const& name, builder::RBuilder<algorithm::IAlgorithm> builder) {
  internal_::registry::registry_.register_algorithm(name, builder);
}

builder::RBuilder<algorithm::IAlgorithm> resolve_algorithm(
    std::string const& name) {
  return internal_::registry::registry_.resolve_algorithm(name);
}

}  // namespace ea::registry

