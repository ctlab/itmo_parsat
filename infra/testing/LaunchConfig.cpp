#include "infra/testing/LaunchConfig.h"

namespace infra::testing {

LaunchConfig& LaunchConfig::set_input_path(std::filesystem::path const& new_input_path) noexcept {
  input_path = new_input_path;
  return *this;
}

LaunchConfig& LaunchConfig::set_config_path(std::filesystem::path const& new_config_path) noexcept {
  config_path = new_config_path;
  return *this;
}

LaunchConfig& LaunchConfig::set_log_config_path(
    std::filesystem::path const& new_log_config_path) noexcept {
  log_config_path = new_log_config_path;
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