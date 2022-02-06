#ifndef ITMO_PARSAT_LAUNCHESDAO_H
#define ITMO_PARSAT_LAUNCHESDAO_H

#include <filesystem>
#include <fstream>
#include <mutex>

#include "core/util/Logger.h"
#include "pqxx/pqxx"

namespace infra::domain {

enum LaunchResult {
  INTERRUPTED,
  ERROR,
  PASSED,
  FAILED,
};

std::string to_string(LaunchResult result);

enum SatResult {
  SAT,
  UNSAT,
  UNKNOWN,
  SAT_ERROR,
};

std::string to_string(SatResult result);

struct LaunchInfo {
  uint32_t launch_id{};
  std::string test_group{};
  std::filesystem::path input_path{};
  std::filesystem::path config_path{};
  std::filesystem::path log_path{};
  std::string branch{};
  std::string commit_hash{};
  LaunchResult result{};
  uint64_t started_at{};
  uint64_t finished_at{};
  std::string description{};
};

class LaunchesDao {
 private:
  void _exec0(std::string const& sql);

 public:
  LaunchesDao(std::string const& host);

  LaunchesDao& add(LaunchInfo const& launch);

  LaunchesDao& remove(uint32_t launch_id);

  [[nodiscard]] bool contains(LaunchInfo& launch);

 private:
  std::mutex _m;
  pqxx::connection _conn;
};

}  // namespace infra::domain

#endif  // ITMO_PARSAT_LAUNCHESDAO_H
