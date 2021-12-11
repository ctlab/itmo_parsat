#ifndef ITMO_PARSAT_LAUNCHCONFIG_H
#define ITMO_PARSAT_LAUNCHCONFIG_H

#include <filesystem>

#include "infra/include/domain/Launches.h"

namespace infra::testing {

struct LaunchConfig {
  bool backdoor_ = false;
  infra::domain::SatResult expected_result_{};
  std::filesystem::path config_path_{};
  std::filesystem::path input_path_{};

 public:
  LaunchConfig() = default;

  LaunchConfig& backdoor(bool backdoor = true) noexcept;

  LaunchConfig& input_path(std::filesystem::path const& input_path) noexcept;

  LaunchConfig& config_path(std::filesystem::path const& config_path) noexcept;

  LaunchConfig& expected_result(infra::domain::SatResult result) noexcept;
};

}  // namespace infra::testing

#endif  // ITMO_PARSAT_LAUNCHCONFIG_H
