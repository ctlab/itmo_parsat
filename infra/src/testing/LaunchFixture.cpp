#include "infra/include/testing/LaunchFixture.h"

#include <fstream>
#include <csignal>
#include <chrono>
#include <thread>

namespace {

LaunchFixture* _fixture = nullptr;

void _sig_handler(int) {
  if (_fixture != nullptr) {
    LOG(INFO) << "Caught interrupt!";
    _fixture->interrupt();
  }
}

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
  std::signal(SIGINT, _sig_handler);
  _fixture = this;
}

LaunchFixture::~LaunchFixture() noexcept {
  std::signal(SIGINT, SIG_DFL);
  _fixture = nullptr;
}

LaunchFixture::Config LaunchFixture::config{};

void LaunchFixture::interrupt() {
  for (auto& exec : execs_) {
    exec->interrupt();
  }
}

void LaunchFixture::SetUpTestSuite() {
  /* Create logs root directory if it does not exist */
  if (!std::filesystem::is_directory(config.working_dir)) {
    LOG(INFO) << "Creating logs root directory: " << config.working_dir;
    std::filesystem::create_directories(config.working_dir);
  }
  /* Prepare resources */
  _prepare_resources();
}

void LaunchFixture::SetUp() {
  launches = std::make_unique<infra::domain::Launches>(config.dbname, config.user, config.password);
}

void LaunchFixture::TearDown() {
  for (auto& exec : execs_) {
    exec->await();
  }
  execs_.clear();
}

void LaunchFixture::_prepare_resources() {
  if (!cnfs.empty()) {
    return;
  }
  CHECK(std::filesystem::exists(config.resources_dir));
  std::vector<std::filesystem::path> result;
  auto cnf_path = config.resources_dir / "cnf";
  for (auto const& entry : std::filesystem::recursive_directory_iterator(cnf_path)) {
    if (entry.is_regular_file() && entry.path().extension() == ".cnf") {
      LOG(INFO) << "Found CNF: " << entry.path();
      cnfs.push_back(std::filesystem::relative(entry.path(), cnf_path));
    }
  }
}

void LaunchFixture::launch(infra::testing::LaunchConfig launch_config) {
  static std::random_device rnd_dev_;
  static std::mt19937 rnd_gen_(rnd_dev_());
  try {
    /* Setup result if unknown */
    if (launch_config.expected_result == infra::domain::UNKNOWN) {
      auto filename = launch_config.input_path.filename().string();
      if (filename.rfind("unsat", 0) == 0) {
        LOG(INFO) << "Deduced UNSAT expected result for " << filename;
        launch_config.expected_result = infra::domain::UNSAT;
      } else if (filename.rfind("sat", 0) == 0) {
        LOG(INFO) << "Deduced SAT expected result for " << filename;
        launch_config.expected_result = infra::domain::SAT;
      } else {
        LOG(WARNING) << "Could not deduce result for " << filename << ", leaving UNKNOWN";
      }
    }

    /* Setup directories */
    std::filesystem::path const& logs_root = config.working_dir / "logs";
    std::filesystem::path const& configs_root = config.working_dir / "configs";
    std::filesystem::create_directories(logs_root);
    std::filesystem::create_directories(configs_root);
    /* Setup files */
    std::string maybe_uniq_str =
        std::to_string(std::time(nullptr)) +
        std::to_string(std::uniform_int_distribution<int>(INT_MIN, INT_MAX)(rnd_gen_));
    std::filesystem::path const& logs_path = logs_root / (maybe_uniq_str + ".txt");
    std::filesystem::path const& config_path = configs_root / (maybe_uniq_str + ".json");
    /* Copy configuration */
    std::filesystem::path real_config_path =
        config.resources_dir / "config" / launch_config.config_path;
    std::filesystem::path real_input_path =
        config.resources_dir / "cnf" / launch_config.input_path;
    std::filesystem::copy_file(real_config_path, config_path);

    // clang-format off
    auto callback = [=] (uint64_t started_at, uint64_t finished_at,
                         int exit_code, bool interrupted) {
      infra::domain::SatResult sat_result = exit_code_to_sat_result(exit_code);
      infra::domain::LaunchResult launch_result
        = _get_launch_result(interrupted, sat_result, launch_config.expected_result);
      LOG(INFO) << "\n\tFinished [" + launch_config.description + "]:"
                << "\n\t\tInput file: " << real_input_path
                << "\n\t\tConfiguration: " << real_config_path
                << "\n\t\tLogs at: " << logs_path
                << "\n\t\tExit code: " << exit_code
                << "\n\t\tSat result: " << infra::domain::to_string(sat_result)
                << "\n\t\tTest result: " << infra::domain::to_string(launch_result);
      launches->add(
        infra::domain::Launch{
          0, real_input_path, real_config_path, logs_path, launch_config.backdoor,
          config.commit, _get_launch_result(interrupted, sat_result, launch_config.expected_result),
          started_at, finished_at, launch_config.description
        }
      );
    };
    if (launch_config.backdoor) {
      execs_.emplace_back(std::make_unique<infra::Execution>(callback,
        logs_path, logs_path, config.executable.string(), "--backdoor",
        "--input", real_input_path.string(),
        "--config", real_config_path.string()
      ));
    } else {
      execs_.emplace_back(std::make_unique<infra::Execution>(callback,
          logs_path, logs_path, config.executable.string(),
          "--input", real_input_path.string(),
          "--config", real_config_path.string()
      ));
    }
    // clang-format on
    LOG(INFO) << "\n\tLaunched [" + launch_config.description + "]:"
              << "\n\t\tInput file: " << real_input_path
              << "\n\t\tConfiguration: " << real_config_path << "\n\t\tLogs at: " << logs_path
              << "\n\t\tExpected result: "
              << infra::domain::to_string(launch_config.expected_result);
  } catch (std::exception const& e) {
    LOG(ERROR) << "Caught exception while trying to start subprocess:\n" << e.what();
  }
}

infra::domain::LaunchResult LaunchFixture::_get_launch_result(
    bool interrupted, infra::domain::SatResult result, infra::domain::SatResult expected) noexcept {
  if (interrupted) {
    return infra::domain::INTERRUPTED;
  }
  return (result == expected || expected == infra::domain::UNKNOWN) ? infra::domain::PASSED
                                                                    : infra::domain::FAILED;
}

std::vector<std::filesystem::path> LaunchFixture::cnfs{};
