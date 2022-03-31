#include "infra/fixture/LaunchFixture.h"

#include <fstream>
#include <chrono>
#include <thread>

namespace {

namespace bp = boost::process;

infra::domain::SatResult exit_code_to_sat_result(int exit_code) {
  switch (exit_code) {
    case 0:
      return infra::domain::UNSAT;
    case 1:
      return infra::domain::SAT;
    case 2:
      return infra::domain::UNKNOWN;
    default:
      return infra::domain::SAT_ERROR;
  }
}

}  // namespace

LaunchFixture::LaunchFixture() {
  _sig_cb =
      core::event::attach([this] { interrupt(); }, core::event::INTERRUPT);
}

infra::fixture::TestingConfiguration LaunchFixture::config{};

void LaunchFixture::interrupt() {
  _exec_manager->interrupt();
  is_interrupted = true;
}

void LaunchFixture::SetUpTestSuite() {
  if (!std::filesystem::is_directory(config.working_dir)) {
    std::filesystem::create_directories(config.working_dir);
  }
  _prepare_resources();
}

void LaunchFixture::SetUp() {
  InfraParametrized::SetUp();
  _info = std::make_unique<infra::domain::LaunchInfo>(config);
  _exec_manager =
      std::make_unique<infra::execution::ExecutionManager>(config.concurrency);
  /* Setup directories */
  _logs_root = config.working_dir / "logs";
  _configs_root = config.working_dir / "configs";
  _num_tests_left = config.num_tests == 0 ? INT32_MAX : config.num_tests;
  std::filesystem::create_directories(_logs_root);
  std::filesystem::create_directories(_configs_root);
}

void LaunchFixture::TearDown() {
  _exec_manager->await();
}

void LaunchFixture::_prepare_resources() {
  if (!cnfs.empty()) {
    return;
  }
  CHECK(std::filesystem::exists(config.resources_dir));
  std::set<std::filesystem::path> result;
  auto cnf_path = config.resources_dir / "cnf";
  for (auto const& entry : std::filesystem::recursive_directory_iterator(
           cnf_path,
           std::filesystem::directory_options::follow_directory_symlink)) {
    if (entry.is_regular_file() && entry.path().extension() == ".cnf") {
      result.insert(std::filesystem::relative(entry.path(), cnf_path));
    }
  }
  cnfs.reserve(result.size());
  for (auto const& path : result) {
    cnfs.push_back(path);
  }

  // Preserve random still determined order of cnfs (for low num_tests numbers).
  std::shuffle(cnfs.begin(), cnfs.end(), std::mt19937(239));
}

std::string LaunchFixture::_generate_uniq_string() {
  static std::random_device rnd_dev;
  static std::mt19937 gen(rnd_dev());
  return std::to_string(std::time(nullptr)) +
         std::to_string(
             std::uniform_int_distribution<int>(INT_MIN, INT_MAX)(gen));
}

void LaunchFixture::launch_one(infra::domain::LaunchConfig& launch_config) {
  /* Setup files */
  std::string salt = _generate_uniq_string();
  std::filesystem::path const& artifact_logs_path =
      _logs_root / (salt + ".txt");
  std::filesystem::path const& artifact_config_path =
      _configs_root / (salt + ".json");
  std::string test_group = _info->get_test_group(launch_config);

  std::filesystem::path config_path =
      config.resources_dir / "config" / launch_config.config;
  std::filesystem::path log_config_path =
      config.resources_dir / "config" / launch_config.log_config;
  std::filesystem::path input_path =
      config.resources_dir / "cnf" / launch_config.input;
  std::filesystem::copy_file(config_path, artifact_config_path);

  infra::domain::LaunchObject launch;
  launch.test_group = test_group;
  launch.input_name = _info->get_input_name(launch_config);
  launch.config_name = launch_config.config;
  launch.log_path = artifact_logs_path;
  launch.branch = config.branch;
  launch.commit_hash = config.commit;
  launch.description = launch_config.description;

  auto callback = [=](uint64_t started_at, uint64_t finished_at, int exit_code,
                      bool interrupted, bool tle) mutable {
    infra::domain::SatResult sat_result = exit_code_to_sat_result(exit_code);
    infra::domain::LaunchResult launch_result = _get_launch_result(
        interrupted, tle, sat_result, launch_config.expected_result);
    LOG(INFO) << "\n\tFinished [" + launch_config.description + "]:"
              << "\n\t\tInput file: " << input_path
              << "\n\t\tConfiguration: " << config_path
              << "\n\t\tLogs at: " << artifact_logs_path
              << "\n\t\tExit code: " << exit_code
              << "\n\t\tSat result: " << infra::domain::to_string(sat_result)
              << "\n\t\tTest result: "
              << infra::domain::to_string(launch_result);
    launch.started_at = started_at;
    launch.finished_at = finished_at;
    launch.result = launch_result;
    if (launch.result == infra::domain::FAILED ||
        launch.result == infra::domain::ERROR ||
        (launch.result == infra::domain::TLE && config.fail_on_tle)) {
      test_failed = true;
    }
    _info->add(launch);
  };

  _exec_manager->execute(
      launch_config.threads_required, callback, artifact_logs_path,
      artifact_logs_path, config.time_limit, config.executable.string(),
      "--verbose", "6", "--input", input_path.string(), "--log-config",
      log_config_path.string(), "--solve-config", config_path.string());

  LOG(INFO) << "\n\tLaunched [" + launch_config.description + "]:"
            << "\n\t\tInput file: " << input_path
            << "\n\t\tConfiguration: " << config_path << " -> "
            << artifact_config_path << "\n\t\tLogs at: " << artifact_logs_path
            << "\n\t\tExpected result: "
            << infra::domain::to_string(launch_config.expected_result);
}

void LaunchFixture::launch(infra::domain::LaunchConfig launch_config) {
  if (is_interrupted || test_failed ||
      !_info->should_be_launched(launch_config)) {
    return;
  }
  if (_num_tests_left-- <= 0) {
    return;
  }
  if (launch_config.expected_result == infra::domain::UNKNOWN) {
    launch_config.expected_result = _info->get_sat_result(launch_config);
  }
  if (launch_config.description.empty()) {
    launch_config.description = config.description;
  }
  std::filesystem::path config_path =
      config.resources_dir / "config" / launch_config.config;
  launch_config.threads_required = _info->get_threads_required(config_path);
  if (launch_config.threads_required > config.concurrency) {
    IPS_WARNING(
        "The test requires " << launch_config.threads_required
                             << " threads, but only " << config.concurrency
                             << " are available.");
    launch_config.threads_required = config.concurrency;
  } else {
    IPS_INFO(
        "OK, test requires " << launch_config.threads_required << " threads");
  }
  for (int i = 0; i < config.repeat_each; ++i) {
    launch_one(launch_config);
  }
}

infra::domain::LaunchResult LaunchFixture::_get_launch_result(
    bool interrupted, bool tle, infra::domain::SatResult result,
    infra::domain::SatResult expected) noexcept {
  if (interrupted) {
    return infra::domain::INTERRUPTED;
  } else if (tle) {
    return infra::domain::TLE;
  } else if (result == infra::domain::SAT_ERROR) {
    return infra::domain::ERROR;
  } else if (result != expected && expected != infra::domain::UNKNOWN) {
    return infra::domain::FAILED;
  } else {
    return infra::domain::PASSED;
  }
}

std::vector<std::filesystem::path> LaunchFixture::cnfs{};

std::atomic_bool LaunchFixture::test_failed = false;

std::atomic_bool LaunchFixture::is_interrupted = false;
