#ifndef ITMO_PARSAT_LAUNCHESDAO_H
#define ITMO_PARSAT_LAUNCHESDAO_H

#include <filesystem>
#include <fstream>
#include <mutex>

#include "util/Logger.h"
#include "pqxx/pqxx"

namespace infra::domain {

enum LaunchResult {
  INTERRUPTED,
  TLE,
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

struct LaunchObject {
  uint32_t launch_id{};
  std::string test_group{};
  std::string input_name{};
  std::string config_name{};
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
  explicit LaunchesDao(std::string const& host);

  LaunchesDao& add(LaunchObject const& launch);

  LaunchesDao& remove(uint32_t launch_id);

  [[nodiscard]] std::optional<uint32_t> get_launch_id(LaunchObject const& launch);

 private:
  std::mutex _m;
  pqxx::connection _conn;
};

}  // namespace infra::domain

#endif  // ITMO_PARSAT_LAUNCHESDAO_H
