#include "infra/include/domain/Launches.h"

namespace infra::domain {

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
    "INSERT INTO Launches(input_path, config_path, log_path, backdoor, commit_hash, started_at, finished_at, result, description)"
    " VALUES (" +
    "\'" + launch.input_path.string() + "\', " +
    "\'" + launch.config_path.string() + "\', " +
    "\'" + launch.log_path.string() + "\', " +
    "\'" + (launch.backdoor ? "true" : "false") + "\', " +
    "\'" + launch.commit_hash + "\', " +
    "to_timestamp(" + std::to_string(launch.started_at) + "), " +
    "to_timestamp(" + std::to_string(launch.finished_at) + "), " +
    "\'" + to_string(launch.result) + "\', " +
    "\'" + launch.description + "\');"
  );
  // clang-format on
  return *this;
}

Launches& Launches::remove(uint32_t launch_id) {
  std::lock_guard<std::mutex> lg(m_);
  _exec0(std::string() + "DELETE FROM Launches WHERE launch_id=" + std::to_string(launch_id) + ";");
  return *this;
}

}  // namespace infra::domain
