#ifndef ITMO_PARSAT_LAUNCHFIXTURE_H
#define ITMO_PARSAT_LAUNCHFIXTURE_H

#include <utility>
#include <memory>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <set>
#include <filesystem>
#include <boost/process.hpp>
#include <glog/logging.h>
#include <random>
#include <gtest/gtest.h>

#include "util/SigHandler.h"
#include "util/EventHandler.h"
#include "util/Semaphore.h"
#include "infra/domain/LaunchInfo.h"
#include "infra/domain/LaunchesDao.h"
#include "infra/domain/LaunchConfig.h"
#include "infra/testing/TestingConfiguration.h"
#include "infra/execution/ExecutionManager.h"

class LaunchFixture : public ::testing::Test {
 private:
 public:
  LaunchFixture();

  ~LaunchFixture() noexcept = default;

  void interrupt();

 protected:
  static void SetUpTestSuite();

 protected:
  void SetUp() override;

  void TearDown() override;

  void launch(infra::domain::LaunchConfig launch_config);

 private:
  static std::string _generate_uniq_string();

  static void _prepare_resources();

  static infra::domain::LaunchResult _get_launch_result(
      bool interrupted, bool tle, infra::domain::SatResult result,
      infra::domain::SatResult expected) noexcept;

 private:
  std::unique_ptr<infra::domain::LaunchInfo> _info;
  std::unique_ptr<infra::execution::ExecutionManager> _exec_manager;
  core::signal::SigHandler _handler;
  core::event::EventCallbackHandle _sig_cb;

 private:
  std::filesystem::path _logs_root;
  std::filesystem::path _configs_root;

 public:
  static infra::testing::TestingConfiguration config;
  static std::atomic_bool test_failed;
  static std::atomic_bool is_interrupted;
  static std::set<std::filesystem::path> cnfs;
};

#endif  // ITMO_PARSAT_LAUNCHFIXTURE_H
