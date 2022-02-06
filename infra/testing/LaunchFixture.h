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

#include "core/util/SigHandler.h"
#include "core/util/EventHandler.h"
#include "infra/domain/LaunchesDao.h"
#include "infra/testing/Execution.h"
#include "infra/testing/LaunchConfig.h"

class LaunchFixture : public ::testing::Test {
 public:
  struct Config {
    bool allow_unspecified_size{};
    bool unsat_only{};
    bool lookup{};
    bool save{};
    int size{};
    uint32_t max_threads = 0;
    std::filesystem::path executable{};
    std::filesystem::path resources_dir{};
    std::filesystem::path working_dir{};
    std::string pg_host = "localhost";
    std::string branch{};
    std::string commit{};
    std::vector<std::string> test_groups;
  };
  static Config config;

 private:
  struct Semaphore {
    void set_max(uint32_t max);

    void acquire(uint32_t num = 1);

    void release(uint32_t num = 1);

   private:
    std::mutex _mutex;
    std::condition_variable _cv;
    uint32_t _current = 0;
    uint32_t _max;
  };

 public:
  LaunchFixture();

  ~LaunchFixture() noexcept override;

  void interrupt();

 protected:
  static void SetUpTestSuite();

 protected:
  void SetUp() override;

  void TearDown() override;

  std::optional<std::shared_ptr<infra::Execution>> launch(
      infra::testing::LaunchConfig launch_config);

  bool _check_if_test_is_done(
      infra::testing::LaunchConfig const& launch_config, infra::domain::LaunchInfo& launch);

  bool _need_to_launch(std::string const& name);

  static int _get_test_size(std::string const& name);

  static std::string _get_test_group(std::string const& name);

  static infra::domain::SatResult _get_sat_result(std::string const& name);

  static std::string _generate_uniq_string();

 private:
  static void _prepare_resources();

  static infra::domain::LaunchResult _get_launch_result(
      bool interrupted, infra::domain::SatResult result,
      infra::domain::SatResult expected) noexcept;

 private:
  std::unique_ptr<infra::domain::LaunchesDao> _launches;
  std::vector<std::shared_ptr<infra::Execution>> _execs;
  core::signal::SigHandler _handler;
  core::event::EventCallbackHandle _sig_cb;

 private:
  std::filesystem::path logs_root;
  std::filesystem::path configs_root;
  static Semaphore semaphore;

 public:
  static std::atomic_bool test_failed;
  static std::set<std::filesystem::path> cnfs;
};

#endif  // ITMO_PARSAT_LAUNCHFIXTURE_H
