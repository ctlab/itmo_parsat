#ifndef ITMO_PARSAT_TESTINGCONFIGURATION_H
#define ITMO_PARSAT_TESTINGCONFIGURATION_H

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace infra::testing {

struct TestingConfiguration {
  bool allow_unspecified_size{};
  bool unsat_only{};
  bool lookup{};
  bool save{};
  int size{};
  uint64_t time_limit_s = UINT64_MAX;
  uint32_t max_threads = 0;
  std::filesystem::path executable{};
  std::filesystem::path resources_dir{};
  std::filesystem::path working_dir{};
  std::string pg_host = "localhost";
  std::string branch{};
  std::string commit{};
  std::vector<std::string> test_groups;
};

}  // namespace infra::testing

#endif  // ITMO_PARSAT_TESTINGCONFIGURATION_H
