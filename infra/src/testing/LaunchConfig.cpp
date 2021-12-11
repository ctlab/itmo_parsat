#include "infra/include/testing/LaunchConfig.h"

namespace infra::testing {

LaunchConfig& LaunchConfig::backdoor(bool backdoor) noexcept {
  backdoor_ = backdoor;
  return *this;
}

LaunchConfig& LaunchConfig::input_path(
    std::filesystem::path const& input_path) noexcept {
  input_path_ = input_path;
  return *this;
}

LaunchConfig& LaunchConfig::config_path(
    std::filesystem::path const& config_path) noexcept {
  config_path_ = config_path;
  return *this;
}

LaunchConfig& LaunchConfig::expected_result(
    infra::domain::SatResult result) noexcept {
  expected_result_ = result;
  return *this;
}

}  // namespace infra::testing