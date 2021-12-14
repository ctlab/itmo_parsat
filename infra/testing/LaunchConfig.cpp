#include "infra/include/testing/LaunchConfig.h"

namespace infra::testing {

LaunchConfig& LaunchConfig::set_backdoor(bool new_backdoor) noexcept {
  backdoor = new_backdoor;
  return *this;
}

LaunchConfig& LaunchConfig::set_input_path(std::filesystem::path const& new_input_path) noexcept {
  input_path = new_input_path;
  return *this;
}

LaunchConfig& LaunchConfig::set_config_path(std::filesystem::path const& new_config_path) noexcept {
  config_path = new_config_path;
  return *this;
}

LaunchConfig& LaunchConfig::set_expected_result(infra::domain::SatResult result) noexcept {
  expected_result = result;
  return *this;
}

LaunchConfig& LaunchConfig::set_description(std::string const& desc) {
  description = desc;
  return *this;
}

}  // namespace infra::testing