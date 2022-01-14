#include "infra/testing/LaunchFixture.h"

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
  _sig_cb = core::sig::register_callback([this](int) { interrupt(); });
}

LaunchFixture::~LaunchFixture() noexcept {}

LaunchFixture::Config LaunchFixture::config{};

void LaunchFixture::interrupt() {
  for (auto& exec : _execs) {
    exec->interrupt();
  }
  test_failed = true;
}

void LaunchFixture::SetUpTestSuite() {
  /* Reset failed flag */
  test_failed = false;

  /* Create logs root directory if it does not exist */
  if (!std::filesystem::is_directory(config.working_dir)) {
    LOG(INFO) << "Creating logs root directory: " << config.working_dir;
    std::filesystem::create_directories(config.working_dir);
  }
  /* Prepare resources */
  _prepare_resources();
}

void LaunchFixture::SetUp() {
  test_failed = false;
  _launches =
      std::make_unique<infra::domain::LaunchesDao>(config.dbname, config.user, config.password);
}

void LaunchFixture::TearDown() {
  _ignore_cnfs.clear();
  for (auto& exec : _execs) {
    exec->await();
  }
  _execs.clear();
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

std::optional<std::shared_ptr<infra::Execution>> LaunchFixture::launch(
    infra::testing::LaunchConfig launch_config) {
  static std::random_device rnd_dev_;
  static std::mt19937 rnd_gen_(rnd_dev_());

  if (test_failed) {
    return {};
  }

  if (_ignore_cnfs.count(launch_config.input_path)) {
    LOG(INFO) << "LaunchInfo for input path " << launch_config.input_path << " is ignored.";
    return {};
  }

  {
    auto it = launch_config.input_path.string().find("@");
    if (it == std::string::npos) {
      LOG(WARNING) << "Test " << launch_config.input_path << " has unspecified size.";
      if (config.size != 2) {
        LOG(INFO) << "LaunchInfo for input path " << launch_config.input_path
                  << " skipped for unknown size.";
        return {};
      }
    }
    int size = launch_config.input_path.string()[it + 1] - '0';
    if (size > config.size) {
      LOG(INFO) << "LaunchInfo for input path " << launch_config.input_path << " with size " << size
                << " is skipped.";
      return {};
    }
  }

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
    std::filesystem::path real_log_config_path =
        config.resources_dir / "config" / launch_config.log_config_path;
    std::filesystem::path real_input_path = config.resources_dir / "cnf" / launch_config.input_path;
    std::filesystem::copy_file(real_config_path, config_path);

    // clang-format off
    auto launch = infra::domain::LaunchInfo{
            0, real_input_path, real_config_path, logs_path,
            config.commit, infra::domain::ERROR,
            0, 0, launch_config.description
        };

    if (config.lookup && _launches->contains(launch)) {
      LOG(INFO) << "\n\tLaunchInfo already done [" << launch_config.description << "]:"
                << "\n\t\tLaunchInfo ID: " << launch.launch_id
                << "\n\t\tInput file: " << real_input_path
                << "\n\t\tConfiguration: " << real_config_path
                << "\n\t\tCommit hash: " << config.commit
                << "\n\t\tLogs at: " << launch.log_path
                << "\n\t\tResult: " << infra::domain::to_string(launch.result);
      return {};
    }

    auto callback = [=] (uint64_t started_at, uint64_t finished_at,
                         int exit_code, bool interrupted) mutable {
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
      launch.started_at = started_at;
      launch.finished_at = finished_at;
      launch.result = _get_launch_result(interrupted, sat_result, launch_config.expected_result);
      if (launch.result == infra::domain::FAILED) {
        test_failed = true;
      }
      if (config.save && launch.result != infra::domain::INTERRUPTED) {
        _launches->add(launch);
      }
    };

    auto result = std::make_shared<infra::Execution>(callback,
      logs_path, logs_path, config.executable.string(),
      "--verbose", "6",  // TODO(dzhiblavi@): make this a parameter
      "--input", real_input_path.string(),
      "--log-config", real_log_config_path.string(),
      "--config", real_config_path.string()
    );
    _execs.emplace_back(result);
    // clang-format on

    LOG(INFO) << "\n\tLaunched [" + launch_config.description + "]:"
              << "\n\t\tInput file: " << real_input_path
              << "\n\t\tConfiguration: " << real_config_path << " -> " << config_path
              << "\n\t\tLogs at: " << logs_path << "\n\t\tExpected result: "
              << infra::domain::to_string(launch_config.expected_result);
    return result;
  } catch (std::exception const& e) {
    LOG(FATAL) << "Caught exception while trying to start subprocess:\n" << e.what();
    __builtin_unreachable();
  }
}

void LaunchFixture::ignore(std::string const& name) {
  _ignore_cnfs.insert(name);
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

std::atomic_bool LaunchFixture::test_failed = false;
