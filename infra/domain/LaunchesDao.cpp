#include "infra/domain/LaunchesDao.h"

namespace {

std::string read_file(std::filesystem::path const& path) {
  std::ifstream ifs(path);
  ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  return std::string(
      std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
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
  } else if (name == "TLE") {
    return TLE;
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
    case TLE:
      return "TLE";
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

LaunchesDao& LaunchesDao::add(LaunchObject const& launch) {
  std::lock_guard<std::mutex> lg(_m);

  // clang-format off
  _exec0(std::string() +
    "INSERT INTO Launches("
    "test_group, input_name, config_name, log_path, branch, commit_hash, started_at, "
    "finished_at, result, description, config)"
    " VALUES (" +
    "'" + launch.test_group + "', " +
    "'" + launch.input_name + "', " +
    "'" + launch.config_name + "', " +
    "'" + launch.log_path.string() + "', " +
    "'" + launch.branch + "', " +
    "'" + launch.commit_hash + "', " +
    "to_timestamp(" + std::to_string(launch.started_at) + "), " +
    "to_timestamp(" + std::to_string(launch.finished_at) + "), " +
    "'" + to_string(launch.result) + "', " +
    "'" + launch.description + "', " +
    "'');"
  );
  // clang-format on
  return *this;
}

LaunchesDao& LaunchesDao::remove(uint32_t launch_id) {
  std::lock_guard<std::mutex> lg(_m);
  _exec0(
      std::string() + "DELETE FROM Launches WHERE launch_id=" +
      std::to_string(launch_id) + ";");
  return *this;
}

std::optional<uint32_t> LaunchesDao::get_launch_id(LaunchObject const& launch) {
  std::lock_guard<std::mutex> lg(_m);
  pqxx::work work(_conn);
  // clang-format off
  auto result = work.exec(std::string() +
    "SELECT launch_id, result FROM Launches WHERE "
    "test_group = '" + launch.test_group + "' AND " +
    "input_name = '" + launch.input_name + "' AND " +
    "config_name = '" + launch.config_name + "' AND " +
    "branch = '" + launch.branch + "' AND " +
    "commit_hash = '" + launch.commit_hash + "';"
  );
  // clang-format on
  if (result.size() == 0) {
    work.commit();
    return std::nullopt;
  } else {
    IPS_VERIFY(result.size() == 1 && bool("Multiple rows for equal launches"));
    work.commit();
    return result.front().at("launch_id").as<uint32_t>();
  }
}

void LaunchesDao::_exec0(std::string const& sql) {
  pqxx::work work(_conn);
  work.exec0(sql);
  work.commit();
}

}  // namespace infra::domain
