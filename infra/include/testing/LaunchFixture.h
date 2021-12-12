#ifndef ITMO_PARSAT_LAUNCHFIXTURE_H
#define ITMO_PARSAT_LAUNCHFIXTURE_H

#include <utility>
#include <memory>
#include <vector>
#include <filesystem>
#include <boost/process.hpp>
#include <glog/logging.h>
#include <random>
#include <gtest/gtest.h>

#include "infra/include/domain/Launches.h"
#include "infra/include/testing/Execution.h"
#include "infra/include/testing/LaunchConfig.h"

class LaunchFixture : public ::testing::Test {
 public:
  struct Config {
    std::filesystem::path executable{};
    std::filesystem::path resources_dir{};
    std::filesystem::path working_dir{};
    std::string commit{};
    std::string dbname{};
    std::string user{};
    std::string password{};
  };
  static Config config;

 public:
  LaunchFixture();

  ~LaunchFixture() noexcept override;

  void interrupt();

 protected:
  static void SetUpTestSuite();

 protected:
  void SetUp() override;

  void TearDown() override;

  void launch(infra::testing::LaunchConfig const& launch_config);

 private:
  void _kill_all_children();

  static void _prepare_resources();

  static infra::domain::LaunchResult _code_to_result(
      bool interrupted, int exit_code, infra::domain::SatResult expected) noexcept;

 private:
  std::unique_ptr<infra::domain::Launches> launches;
  std::vector<std::unique_ptr<infra::Execution>> execs_;

 public:
  static std::vector<std::filesystem::path> cnfs_;
};

#endif  // ITMO_PARSAT_LAUNCHFIXTURE_H
