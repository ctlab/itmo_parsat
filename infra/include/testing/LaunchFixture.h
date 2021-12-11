#ifndef ITMO_PARSAT_LAUNCHFIXTURE_H
#define ITMO_PARSAT_LAUNCHFIXTURE_H

#include <utility>
#include <memory>
#include <vector>
#include <filesystem>
#include <boost/process.hpp>
#include <glog/logging.h>
#include <gtest/gtest.h>

#include "infra/include/domain/Launches.h"

class LaunchFixture : public ::testing::Test {
 public:
  struct Config {
    std::filesystem::path executable{};
    std::filesystem::path resources_dir{};
    std::filesystem::path working_dir{};
    std::string branch{};
    std::string dbname{};
    std::string user{};
    std::string password{};
  };
  static Config config;

 public:
  struct LaunchConfig {
    bool backdoor_ = false;
    infra::domain::Result expected_result_{};
    std::filesystem::path config_path_{};
    std::filesystem::path input_path_{};

   public:
    LaunchConfig() = default;

    LaunchConfig& backdoor(bool backdoor = true) noexcept;

    LaunchConfig& input_path(std::filesystem::path const& input_path) noexcept;

    LaunchConfig& config_path(std::filesystem::path const& config_path) noexcept;

    LaunchConfig& expected_result(infra::domain::Result result) noexcept;
  };

 public:
  LaunchFixture();

  ~LaunchFixture() noexcept override;

  static void prepare();

  void interrupt();

 protected:
  void SetUp() override;

  void TearDown() override;

  void launch(LaunchConfig const& launch_config);

 private:
  void _kill_all_children();

 private:
  struct Launch {
    boost::process::child proc;
    std::filesystem::path logs_path;
    std::filesystem::path config_path;
    std::filesystem::path input_path;
    std::string branch;
    infra::domain::Result expected_result;
    bool backdoor = false;
    bool interrupted = false;

   public:
    // clang-format off
    template <typename... Args>
    explicit Launch(std::filesystem::path logs_path,
                    std::filesystem::path config_path,
                    std::filesystem::path input_path,
                    std::string branch,
                    infra::domain::Result result,
                    bool backdoor, Args&&... args)
        : proc(std::forward<Args>(args)...), logs_path(std::move(logs_path)),
          config_path(std::move(config_path)), input_path(std::move(input_path)),
          branch(std::move(branch)), expected_result(result), backdoor(backdoor) {}
    // clang-format on

    void interrupt();

    void await();

    void save_to_db(infra::domain::Launches& launches);
  };

 private:
  std::unique_ptr<infra::domain::Launches> launches;
  std::vector<Launch> procs_;
};

#endif  // ITMO_PARSAT_LAUNCHFIXTURE_H
