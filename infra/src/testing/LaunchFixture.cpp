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
  LOG(INFO) << "Preparing!";
  if (!cnfs_.empty()) {
    return;
  }
  CHECK(std::filesystem::exists(config.resources_dir));
  std::vector<std::filesystem::path> result;
  auto cnf_path = config.resources_dir / "cnf";
  for (auto const& entry : std::filesystem::recursive_directory_iterator(cnf_path)) {
    if (entry.is_regular_file() && entry.path().extension() == ".cnf") {
      LOG(INFO) << "Found CNF: " << entry.path();
      cnfs_.push_back(std::filesystem::relative(entry.path(), cnf_path));
    }
  }
}

void LaunchFixture::launch(infra::testing::LaunchConfig launch_config) {
  static std::random_device rnd_dev_;
  static std::mt19937 rnd_gen_(rnd_dev_());
  try {
    /* Setup result if unknown */
    if (launch_config.expected_result_ == infra::domain::UNKNOWN) {
      auto filename = launch_config.input_path_.filename().string();
      if (filename.rfind("unsat", 0) == 0) {
        LOG(INFO) << "Deduced UNSAT expected result for " << filename;
        launch_config.expected_result_ = infra::domain::UNSAT;
      } else if (filename.rfind("sat", 0) == 0) {
        LOG(INFO) << "Deduced SAT expected result for " << filename;
        launch_config.expected_result_ = infra::domain::SAT;
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
        config.resources_dir / "config" / launch_config.config_path_;
    std::filesystem::path real_input_path =
        config.resources_dir / "cnf" / launch_config.input_path_;
    std::filesystem::copy_file(real_config_path, config_path);

    // clang-format off
    auto callback = [=] (uint64_t started_at, uint64_t finished_at,
                         int exit_code, bool interrupted) {
      LOG(INFO) << "Finished: " << real_input_path << ' '
                << started_at << ' ' << finished_at << ' '
                << exit_code << ' ' << interrupted;
      launches->add(
        infra::domain::Launch{
          0, real_input_path, real_config_path, logs_path, launch_config.backdoor_,
          config.commit, _code_to_result(interrupted, exit_code, launch_config.expected_result_),
          started_at, finished_at
        }
      );
    };
    if (launch_config.backdoor_) {
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
    LOG(INFO) << "Launched. See logs at " << logs_path;
  } catch (std::exception const& e) {
    LOG(ERROR) << "Caught exception while trying to start subprocess:\n" << e.what();
  }
}

infra::domain::LaunchResult LaunchFixture::_code_to_result(
    bool interrupted, int exit_code, infra::domain::SatResult expected) noexcept {
  if (interrupted) {
    return infra::domain::INTERRUPTED;
  }

  infra::domain::SatResult sat_result;
  switch (exit_code) {
    case 0:
      sat_result = infra::domain::UNSAT;
      break;
    case 1:
      sat_result = infra::domain::SAT;
      break;
    case 2:
      sat_result = infra::domain::UNKNOWN;
      break;
    default:
      return infra::domain::ERROR;
  }

  return sat_result == expected ? infra::domain::PASSED : infra::domain::FAILED;
}

std::vector<std::filesystem::path> LaunchFixture::cnfs_{};
