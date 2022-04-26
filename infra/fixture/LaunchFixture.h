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

#include "infra/fixture/TestingConfiguration.h"
#include "core/proto/solve_config.pb.h"
#include "core/tests/common/util.h"
#include "util/SigHandler.h"
#include "util/EventHandler.h"
#include "util/Semaphore.h"
#include "infra/domain/LaunchInfo.h"
#include "infra/domain/LaunchesDao.h"
#include "infra/domain/LaunchConfig.h"
#include "infra/execution/ExecutionManager.h"

DEFINE_PARAMETRIZED_TEST(
    InfraParametrized, int /* cpu */, std::string /* with/without painless */,
    std::string /* solver */, std::string /* reduce */, std::string /* rbs */,
    std::string /* (e|g)a */);

class LaunchFixture : public InfraParametrized {
 private:
 public:
  LaunchFixture();

  ~LaunchFixture() noexcept override = default;

  void interrupt();

 protected:
  static void SetUpTestSuite();

 protected:
  void SetUp() override;

  void TearDown() override;

  void launch(infra::domain::LaunchConfig launch_config);

 private:
  void launch_one(infra::domain::LaunchConfig& launch_config);

  static std::string _generate_uniq_string();

  static void _prepare_resources();

  static infra::domain::LaunchResult _get_launch_result(
      bool interrupted, bool tle, infra::domain::SatResult result,
      infra::domain::SatResult expected) noexcept;

 private:
  std::unique_ptr<infra::domain::LaunchInfo> _info;
  std::unique_ptr<infra::execution::ExecutionManager> _exec_manager;
  util::signal::SigHandler _handler;
  util::event::EventCallbackHandle _sig_cb;

 private:
  int _num_tests_left = 0;
  std::filesystem::path _logs_root;
  std::filesystem::path _configs_root;

 public:
  static infra::fixture::TestingConfiguration config;
  static std::atomic_bool test_failed;
  static std::atomic_bool is_interrupted;
  static std::vector<std::filesystem::path> cnfs;
};

#endif  // ITMO_PARSAT_LAUNCHFIXTURE_H
