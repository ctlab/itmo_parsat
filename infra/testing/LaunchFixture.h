#ifndef ITMO_PARSAT_LAUNCHFIXTURE_H
#define ITMO_PARSAT_LAUNCHFIXTURE_H

#include <utility>
#include <memory>
#include <vector>
#include <set>
#include <filesystem>
#include <boost/process.hpp>
#include <glog/logging.h>
#include <random>
#include <gtest/gtest.h>

#include "core/util/SigHandler.h"
#include "infra/domain/Launches.h"
#include "infra/testing/Execution.h"
#include "infra/testing/LaunchConfig.h"

class LaunchFixture : public ::testing::Test {
 public:
  struct Config {
    bool lookup{};
    bool save{};
    int size{};
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

  void ignore(std::string const& name);

 protected:
  static void SetUpTestSuite();

  static void TearDownTestSuite();

 protected:
  void SetUp() override;

  void TearDown() override;

  std::optional<std::shared_ptr<infra::Execution>> launch(
      infra::testing::LaunchConfig launch_config);

 private:
  static void _prepare_resources();

  static infra::domain::LaunchResult _get_launch_result(
      bool interrupted, infra::domain::SatResult result,
      infra::domain::SatResult expected) noexcept;

 private:
  std::unique_ptr<infra::domain::Launches> _launches;
  std::vector<std::shared_ptr<infra::Execution>> _execs;
  std::set<std::string> _ignore_cnfs;
  core::SigHandler _handler;
  core::SigHandler::handle_t _sig_cb;
  static std::atomic_bool test_failed;

 public:
  static std::vector<std::filesystem::path> cnfs;
};

#endif  // ITMO_PARSAT_LAUNCHFIXTURE_H
