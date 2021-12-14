#ifndef ITMO_PARSAT_LAUNCHCONFIG_H
#define ITMO_PARSAT_LAUNCHCONFIG_H

#include <filesystem>

#include "infra/domain/Launches.h"

namespace infra::testing {

struct LaunchConfig {
  bool backdoor = false;
  infra::domain::SatResult expected_result{};
  std::filesystem::path config_path{};
  std::filesystem::path input_path{};
  std::string description{};

 public:
  LaunchConfig() = default;

  LaunchConfig& set_backdoor(bool backdoor = true) noexcept;

  LaunchConfig& set_input_path(std::filesystem::path const& input_path) noexcept;

  LaunchConfig& set_config_path(std::filesystem::path const& config_path) noexcept;

  LaunchConfig& set_expected_result(infra::domain::SatResult result) noexcept;

  LaunchConfig& set_description(std::string const& desc);
};

}  // namespace infra::testing

#endif  // ITMO_PARSAT_LAUNCHCONFIG_H
