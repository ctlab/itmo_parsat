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

void LaunchFixture::Semaphore::set_max(uint32_t max_value) {
  max = max_value;
}

void LaunchFixture::Semaphore::acquire(uint32_t num) {
  IPS_VERIFY(num <= max);
  for (;;) {
    std::unique_lock<std::mutex> ul(_mutex);
    _cv.wait(ul, [this, num] { return _current + num <= max; });
    if (_current + num <= max) {
      _current += num;
      break;
    }
  }
}

void LaunchFixture::Semaphore::release(uint32_t num) {
  {
    std::lock_guard<std::mutex> lg(_mutex);
    _current -= num;
  }
  _cv.notify_all();
}

LaunchFixture::LaunchFixture() {
  _sig_cb = core::event::attach([this] { interrupt(); }, core::event::INTERRUPT);
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
    std::filesystem::create_directories(config.working_dir);
  }

  /* Prepare resources */
  _prepare_resources();

  /* Set up semaphore */
  uint32_t concurrency = config.max_threads;
  if (concurrency == 0) {
    concurrency = std::thread::hardware_concurrency();
  }
  semaphore.set_max(concurrency);
  IPS_INFO("Set max concurrency to " << concurrency);
}

void LaunchFixture::SetUp() {
  /* Setup DB connection only if it is required. */
  if (config.save || config.lookup) {
    _launches = std::make_unique<infra::domain::LaunchesDao>(/* host = */ config.pg_host);
  }
  /* Setup directories */
  logs_root = config.working_dir / "logs";
  configs_root = config.working_dir / "configs";
  std::filesystem::create_directories(logs_root);
  std::filesystem::create_directories(configs_root);
}

void LaunchFixture::TearDown() {
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
  for (auto const& entry : std::filesystem::recursive_directory_iterator(
           cnf_path, std::filesystem::directory_options::follow_directory_symlink)) {
    if (entry.is_regular_file() && entry.path().extension() == ".cnf") {
      cnfs.insert(std::filesystem::relative(entry.path(), cnf_path));
    }
  }
}

std::string LaunchFixture::_get_test_group(std::string const& name) {
  return std::filesystem::path(name).parent_path().filename();
}

bool LaunchFixture::_need_to_launch(std::string const& name) {
  if (test_failed) {
    return false;
  }
  if (std::find(config.test_groups.begin(), config.test_groups.end(), _get_test_group(name)) ==
      config.test_groups.end()) {
    return false;
  }
  if (_get_sat_result(name) != infra::domain::UNSAT && config.unsat_only) {
    return false;
  }
  int test_size = _get_test_size(name);
  if (test_size == -1 && !config.allow_unspecified_size) {
    return false;
  }
  if (test_size > config.size) {
    return false;
  }
  return true;
}

int LaunchFixture::_get_test_size(std::string const& name) {
  auto it = name.find("@");
  if (it == std::string::npos) {
    return -1;
  } else {
    return name[it + 1] - '0';
  }
}

infra::domain::SatResult LaunchFixture::_get_sat_result(std::string const& name) {
  auto filename = std::filesystem::path(name).filename().string();
  if (filename.rfind("unsat", 0) == 0) {
    return infra::domain::UNSAT;
  } else if (filename.rfind("sat", 0) == 0) {
    return infra::domain::SAT;
  } else {
    LOG(WARNING) << "Could not deduce result for " << filename << ", leaving UNKNOWN";
    return infra::domain::UNKNOWN;
  }
}

bool LaunchFixture::_check_if_test_is_done(
    infra::testing::LaunchConfig const& launch_config, infra::domain::LaunchInfo& launch) {
  if (config.lookup && _launches->contains(launch)) {
    LOG(INFO) << "\n\tLaunchInfo already done [" << launch_config.description << "]:"
              << "\n\t\tLaunchInfo ID: " << launch.launch_id
              << "\n\t\tCommit hash: " << config.commit << "\n\t\tLogs at: " << launch.log_path
              << "\n\t\tResult: " << infra::domain::to_string(launch.result);
    return true;
  } else {
    return false;
  }
}

std::string LaunchFixture::_generate_uniq_string() {
  static std::random_device rnd_dev;
  static std::mt19937 gen(rnd_dev());
  return std::to_string(std::time(nullptr)) +
         std::to_string(std::uniform_int_distribution<int>(INT_MIN, INT_MAX)(gen));
}

std::optional<std::shared_ptr<infra::Execution>> LaunchFixture::launch(
    infra::testing::LaunchConfig launch_config) {
  // Check if this test needs to be launched
  if (!_need_to_launch(launch_config.input)) {
    return {};
  }

  if (launch_config.threads_required > config.max_threads) {
    IPS_WARNING(
        "The test requires " << launch_config.threads_required << " threads, but only "
                             << config.max_threads << " are available, thus skipped");
    return {};
  }

  // Setup result if unknown
  if (launch_config.expected_result == infra::domain::UNKNOWN) {
    launch_config.expected_result = _get_sat_result(launch_config.input);
  }

  /* Setup files */
  std::string salt = _generate_uniq_string();
  std::filesystem::path const& artifact_logs_path = logs_root / (salt + ".txt");
  std::filesystem::path const& artifact_config_path = configs_root / (salt + ".json");
  std::string test_group = _get_test_group(launch_config.input);

  // clang-format off
  std::filesystem::path config_path = config.resources_dir / "config" / launch_config.config;
  std::filesystem::path log_config_path = config.resources_dir / "config" / launch_config.log_config;
  std::filesystem::path input_path = config.resources_dir / "cnf" / launch_config.input;
  std::filesystem::copy_file(config_path, artifact_config_path);

  auto launch = infra::domain::LaunchInfo{
          0, test_group, input_path, config_path, artifact_logs_path,
          config.branch, config.commit, infra::domain::ERROR,
          0, 0, launch_config.description
      };

  if (_check_if_test_is_done(launch_config, launch)) {
    return {};
  }

  auto callback = [=] (uint64_t started_at, uint64_t finished_at,
                       int exit_code, bool interrupted) mutable {
    infra::domain::SatResult sat_result = exit_code_to_sat_result(exit_code);
    infra::domain::LaunchResult launch_result
      = _get_launch_result(interrupted, sat_result, launch_config.expected_result);
    LOG(INFO) << "\n\tFinished [" + launch_config.description + "]:"
              << "\n\t\tInput file: " << input_path
              << "\n\t\tConfiguration: " << config_path
              << "\n\t\tLogs at: " << artifact_logs_path
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
    /* Release semaphore */
    semaphore.release(launch_config.threads_required);
  };

  /* Acquire semaphore */
  semaphore.acquire(launch_config.threads_required);
  IPS_INFO("Reproduce:\n" <<
    config.executable.string() << " --verbose 6 --input " << input_path.string() <<
    " --log-config " << log_config_path.string() << " --config " << config_path.string()
  );

  auto result = std::make_shared<infra::Execution>(callback,
    artifact_logs_path, artifact_logs_path, config.executable.string(),
    "--verbose", "6",
    "--input", input_path.string(),
    "--log-config", log_config_path.string(),
    "--config", config_path.string()
  );
  _execs.emplace_back(result);
  // clang-format on

  LOG(INFO) << "\n\tLaunched [" + launch_config.description + "]:"
            << "\n\t\tInput file: " << input_path << "\n\t\tConfiguration: " << config_path
            << " -> " << artifact_config_path << "\n\t\tLogs at: " << artifact_logs_path
            << "\n\t\tExpected result: " << infra::domain::to_string(launch_config.expected_result);
  return result;
}

infra::domain::LaunchResult LaunchFixture::_get_launch_result(
    bool interrupted, infra::domain::SatResult result, infra::domain::SatResult expected) noexcept {
  if (interrupted) {
    return infra::domain::INTERRUPTED;
  }
  return (result == expected || expected == infra::domain::UNKNOWN) ? infra::domain::PASSED
                                                                    : infra::domain::FAILED;
}

std::set<std::filesystem::path> LaunchFixture::cnfs{};

std::atomic_bool LaunchFixture::test_failed = false;

LaunchFixture::Semaphore LaunchFixture::semaphore{};
