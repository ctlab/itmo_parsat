#include "LaunchConfig.h"

namespace infra::domain {

LaunchConfig& LaunchConfig::set_input_path(std::filesystem::path const& new_input) noexcept {
  input = new_input;
  return *this;
}

LaunchConfig& LaunchConfig::set_config_path(std::filesystem::path const& new_config) noexcept {
  config = new_config;
  return *this;
}

LaunchConfig& LaunchConfig::set_log_config_path(
    std::filesystem::path const& new_log_config) noexcept {
  log_config = new_log_config;
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

}  // namespace infra::domain
