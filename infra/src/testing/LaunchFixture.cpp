#include "infra/include/testing/LaunchFixture.h"

#include <fstream>
#include <csignal>
#include <chrono>
#include <thread>

namespace {

LaunchFixture* _fixture = nullptr;

void _sig_handler(int) {
  if (_fixture != nullptr) {
    _fixture->interrupt();
  }
}

}  // namespace

LaunchFixture::LaunchFixture() {
  std::signal(SIGINT, _sig_handler);
  _fixture = this;
}

LaunchFixture::~LaunchFixture() noexcept {
  std::signal(SIGINT, SIG_DFL);
  _fixture = nullptr;
}

LaunchFixture::Config LaunchFixture::config{};

void LaunchFixture::interrupt() {
  _kill_all_children();
}

LaunchFixture::LaunchConfig& LaunchFixture::LaunchConfig::backdoor(bool backdoor) noexcept {
  backdoor_ = backdoor;
  return *this;
}

LaunchFixture::LaunchConfig& LaunchFixture::LaunchConfig::input_path(
    std::filesystem::path const& input_path) noexcept {
  input_path_ = input_path;
  return *this;
}

LaunchFixture::LaunchConfig& LaunchFixture::LaunchConfig::config_path(
    std::filesystem::path const& config_path) noexcept {
  config_path_ = config_path;
  return *this;
}

LaunchFixture::LaunchConfig& LaunchFixture::LaunchConfig::expected_result(
    infra::domain::Result result) noexcept {
  expected_result_ = result;
  return *this;
}

void LaunchFixture::prepare() {
  /* Create logs root directory if it does not exist */
  if (!std::filesystem::is_directory(config.working_dir)) {
    LOG(INFO) << "Creating logs root directory: " << config.working_dir;
    std::filesystem::create_directories(config.working_dir);
  }
}

void LaunchFixture::SetUp() {
  launches = std::make_unique<infra::domain::Launches>(config.dbname, config.user, config.password);
}

void LaunchFixture::TearDown() {
  for (auto& proc : procs_) {
    proc.await();
    proc.save_to_db(*launches);
  }
  procs_.clear();
}

void LaunchFixture::_kill_all_children() {
  for (auto& proc : procs_) {
    proc.interrupt();
    proc.save_to_db(*launches);
  }
  procs_.clear();
}

void LaunchFixture::launch(LaunchConfig const& launch_config) {
  namespace bp = boost::process;
  try {
    /* Setup directories */
    std::filesystem::path const& logs_root = config.working_dir / "logs";
    std::filesystem::path const& configs_root = config.working_dir / "configs";
    std::filesystem::create_directories(logs_root);
    std::filesystem::create_directories(configs_root);
    /* Setup files */
    std::string timestamp = std::to_string(std::time(nullptr));
    std::filesystem::path const& logs_path = logs_root / (timestamp + ".txt");
    std::filesystem::path const& config_path = configs_root / (timestamp + ".json");
    /* Copy configuration */
    std::filesystem::path real_config_path =
        config.resources_dir / "config" / launch_config.config_path_;
    std::filesystem::path real_input_path =
        config.resources_dir / "cnf" / launch_config.input_path_;
    std::filesystem::copy_file(real_config_path, config_path);

    // clang-format off
    if (launch_config.backdoor_) {
      procs_.emplace_back(logs_path, config_path, real_input_path, launch_config.expected_result_, true,
        config.executable.string(), "--backdoor",
        "--input", real_input_path.string(), "--config", real_config_path.string(),
        boost::process::std_out > logs_path, boost::process::std_err > logs_path
      );
    } else {
      procs_.emplace_back(logs_path, config_path, real_input_path, launch_config.expected_result_, false,
        config.executable.string(),
        "--input", real_input_path.string(), "--config", real_config_path.string(),
        boost::process::std_out > logs_path, boost::process::std_err > logs_path
      );
    }
    // clang-format on
    LOG(INFO) << "Launched. See logs at " << logs_path;
  } catch (std::exception const& e) {
    LOG(ERROR) << "Caught exception while trying to start subprocess:\n" << e.what();
  }
}

void LaunchFixture::Launch::interrupt() {
  using namespace std::chrono_literals;
  if (proc.running()) {
    interrupted = true;
    auto handle = proc.native_handle();
    kill(handle, SIGINT);
    std::this_thread::sleep_for(100ms);
    kill(handle, SIGINT);
    proc.join();
  }
}

void LaunchFixture::Launch::await() {
  if (proc.running()) {
    proc.join();
  }
}

void LaunchFixture::Launch::save_to_db(infra::domain::Launches& db_launches) {
  infra::domain::Result result = infra::domain::INTERRUPTED;
  if (!interrupted) {
    switch (proc.exit_code()) {
      case 0:
        result = infra::domain::UNSAT;
        break;
      case 1:
        result = infra::domain::SAT;
        break;
      case 2:
        result = infra::domain::UNKNOWN;
        break;
      default:
        result = infra::domain::ERROR;
        break;
    }
    ASSERT_EQ(result, expected_result);
  } else {
    LOG(WARNING) << "Solution has been interrupted.";
  }
  db_launches.add(
      infra::domain::Launch{0, input_path, config_path, logs_path, backdoor, "", result});
}
