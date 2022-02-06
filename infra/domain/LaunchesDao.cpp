#include "infra/domain/LaunchesDao.h"

namespace {

std::string read_file(std::filesystem::path const& path) {
  std::ifstream ifs(path);
  ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  return std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
}

}  // namespace

namespace infra::domain {

LaunchResult from_string(std::string const& name) {
  if (name == "ERROR") {
    return ERROR;
  } else if (name == "INTERRUPTED") {
    return INTERRUPTED;
  } else if (name == "PASSED") {
    return PASSED;
  } else if (name == "FAILED") {
    return FAILED;
  }
  IPS_ERROR("Failed to convert LaunchResult from string: '" << name << "'");
  std::terminate();
}

std::string to_string(LaunchResult result) {
  switch (result) {
    case ERROR:
      return "ERROR";
    case INTERRUPTED:
      return "INTERRUPTED";
    case PASSED:
      return "PASSED";
    case FAILED:
      return "FAILED";
  }
  std::terminate();
}

std::string to_string(SatResult result) {
  switch (result) {
    case SAT:
      return "SAT";
    case UNSAT:
      return "UNSAT";
    case UNKNOWN:
      return "UNKNOWN";
    case SAT_ERROR:
      return "SAT_ERROR";
  }
  std::terminate();
}

LaunchesDao::LaunchesDao(std::string const& host)
    : _conn(std::string("dbname=infra_db user=ips password=ips host=") + host) {
  IPS_INFO("Connecting to infra_db on host '" << host << "'");
}

LaunchesDao& LaunchesDao::add(LaunchInfo const& launch) {
  std::lock_guard<std::mutex> lg(_m);

  // clang-format off
  _exec0(std::string() +
    "INSERT INTO Launches("
    "test_group, input_name, config_name, log_path, branch, commit_hash, started_at, "
    "finished_at, result, description, config)"
    " VALUES (" +
    "'" + launch.test_group + "', " +
    "'" + launch.input_path.filename().string() + "', " +
    "'" + launch.config_path.filename().string() + "', " +
    "'" + launch.log_path.string() + "', " +
    "'" + launch.branch + "', " +
    "'" + launch.commit_hash + "', " +
    "to_timestamp(" + std::to_string(launch.started_at) + "), " +
    "to_timestamp(" + std::to_string(launch.finished_at) + "), " +
    "'" + to_string(launch.result) + "', " +
    "'" + launch.description + "', " +
    "'" + read_file(launch.config_path) + "');"
  );
  // clang-format on
  return *this;
}

LaunchesDao& LaunchesDao::remove(uint32_t launch_id) {
  std::lock_guard<std::mutex> lg(_m);
  _exec0(std::string() + "DELETE FROM Launches WHERE launch_id=" + std::to_string(launch_id) + ";");
  return *this;
}

bool LaunchesDao::contains(LaunchInfo& launch) {
  std::lock_guard<std::mutex> lg(_m);
  pqxx::work work(_conn);
  // clang-format off
  auto result = work.exec(std::string() +
    "SELECT launch_id, log_path, result FROM Launches WHERE "
    "test_group = '" + launch.test_group + "' AND " +
    "input_name = '" + launch.input_path.filename().string() + "' AND " +
    "config_name = '" + launch.config_path.filename().string() + "' AND " +
    "commit_hash = '" + launch.commit_hash + "' AND " +
    "description = '" + launch.description + "';"
  );
  // clang-format on
  if (result.size() == 0) {
    work.commit();
    return false;
  } else {
    IPS_VERIFY(result.size() == 1 && bool("Multiple rows for equal launches"));
    launch.launch_id = result.front()[0].as<size_t>(0);
    launch.log_path = result.front()[1].as<std::string>("UNKNOWN");
    launch.result = from_string(result.front()[2].as<std::string>("UNKNOWN"));
    work.commit();
    return true;
  }
}

void LaunchesDao::_exec0(std::string const& sql) {
  pqxx::work work(_conn);
  work.exec0(sql);
  work.commit();
}

}  // namespace infra::domain
