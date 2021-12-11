#ifndef ITMO_PARSAT_LAUNCHES_H
#define ITMO_PARSAT_LAUNCHES_H

#include <filesystem>
#include "infra/include/domain/PGDB.h"

namespace infra::domain {

enum LaunchResult {
  INTERRUPTED,
  ERROR,
  PASSED,
  FAILED,
};

enum SatResult {
  SAT,
  UNSAT,
  UNKNOWN,
};

std::string to_string(LaunchResult result);

struct Launch {
  uint32_t launch_id{};
  std::filesystem::path input_path{};
  std::filesystem::path config_path{};
  std::filesystem::path log_path{};
  bool backdoor{};
  std::string commit_hash{};
  LaunchResult result{};
  uint64_t started_at{};
  uint64_t finished_at{};
};

class Launches : PGDB {
 public:
  Launches(std::string const& dbname, std::string const& user, std::string const& password);

  Launches& add(Launch const& launch);

  Launches& remove(uint32_t launch_id);
};

}  // namespace infra::domain

#endif  // ITMO_PARSAT_LAUNCHES_H
