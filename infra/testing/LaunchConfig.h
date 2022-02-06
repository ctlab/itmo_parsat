#ifndef ITMO_PARSAT_LAUNCHCONFIG_H
#define ITMO_PARSAT_LAUNCHCONFIG_H

#include <filesystem>

#include "infra/domain/LaunchesDao.h"

namespace infra::testing {

struct LaunchConfig {
  infra::domain::SatResult expected_result{};
  std::string config{};
  std::string log_config{};
  std::string input{};
  std::string description{};
  uint32_t threads_required = 1;

 public:
  LaunchConfig() = default;

  LaunchConfig& set_input_path(std::filesystem::path const& input) noexcept;

  LaunchConfig& set_config_path(std::filesystem::path const& config) noexcept;

  LaunchConfig& set_log_config_path(std::filesystem::path const& config) noexcept;

  LaunchConfig& set_expected_result(infra::domain::SatResult result) noexcept;

  LaunchConfig& set_description(std::string const& desc);

  LaunchConfig& set_threads_required(uint32_t threads);
};

}  // namespace infra::testing

#endif  // ITMO_PARSAT_LAUNCHCONFIG_H
