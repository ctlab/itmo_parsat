#include "infra/include/domain/Launches.h"

namespace infra::domain {

std::string to_string(Result result) {
  switch (result) {
    case ERROR:
      return "ERROR";
    case INTERRUPTED:
      return "INTERRUPTED";
    case SAT:
      return "SAT";
    case UNSAT:
      return "UNSAT";
    case UNKNOWN:
      return "UNKNOWN";
  }
  std::terminate();
}

Launches::Launches(std::string const& dbname, std::string const& user, std::string const& password)
    : PGDB(dbname, user, password) {}

Launches& Launches::add(Launch const& launch) {
  // clang-format off
  _exec0(std::string() +
    "INSERT INTO Launches(input_path, config_path, log_path, backdoor, commit_hash, result) VALUES (" +
    "\'" + launch.input_path.string() + "\', " +
    "\'" + launch.config_path.string() + "\', " +
    "\'" + launch.log_path.string() + "\', " +
    "\'" + (launch.backdoor ? "true" : "false") + "\', " +
    "\'" + launch.commit_hash + "\', " +
    "\'" + to_string(launch.result) + "\');"
  );
  // clang-format on
  return *this;
}

Launches& Launches::remove(uint32_t launch_id) {
  _exec0(std::string() + "DELETE FROM Launches WHERE launch_id=" + std::to_string(launch_id) + ";");
  return *this;
}

}  // namespace infra::domain
