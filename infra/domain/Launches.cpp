#include "infra/domain/Launches.h"

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

Launches::Launches(std::string const& dbname, std::string const& user, std::string const& password)
    : PGDB(dbname, user, password) {}

Launches& Launches::add(Launch const& launch) {
  std::lock_guard<std::mutex> lg(m_);
  // clang-format off
  _exec0(std::string() +
    "INSERT INTO Launches(input_path, config_path, log_path, commit_hash, started_at, finished_at, result, description)"
    " VALUES (" +
    "'" + launch.input_path.string() + "', " +
    "'" + launch.config_path.string() + "', " +
    "'" + launch.log_path.string() + "', " +
    "'" + launch.commit_hash + "', " +
    "to_timestamp(" + std::to_string(launch.started_at) + "), " +
    "to_timestamp(" + std::to_string(launch.finished_at) + "), " +
    "'" + to_string(launch.result) + "', " +
    "'" + launch.description + "');"
  );
  // clang-format on
  return *this;
}

Launches& Launches::remove(uint32_t launch_id) {
  std::lock_guard<std::mutex> lg(m_);
  _exec0(std::string() + "DELETE FROM Launches WHERE launch_id=" + std::to_string(launch_id) + ";");
  return *this;
}

bool Launches::contains(Launch& launch) {
  std::lock_guard<std::mutex> lg(m_);
  pqxx::work work(conn_);
  // clang-format off
  auto result = work.exec(std::string() +
    "SELECT launch_id, log_path, result FROM Launches WHERE "
    "input_path = '" + launch.input_path.string() + "' AND " +
    "config_path = '" + launch.config_path.string() + "' AND " +
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

}  // namespace infra::domain
