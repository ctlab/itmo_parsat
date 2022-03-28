#ifndef ITMO_PARSAT_TESTINGCONFIGURATION_H
#define ITMO_PARSAT_TESTINGCONFIGURATION_H

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace infra::fixture {

struct TestingConfiguration {
  bool allow_unspecified_size = true;
  bool unsat_only = false;
  bool lookup = false;
  bool save = false;
  bool fail_on_tle = false;
  int size = 0;
  int repeat_each = 1;
  uint32_t concurrency = 0;
  uint64_t time_limit = 0;
  std::string description;
  std::string pg_host = "localhost";
  std::string branch;
  std::string commit;
  std::filesystem::path executable;
  std::filesystem::path resources_dir;
  std::filesystem::path working_dir;
  std::vector<std::string> test_groups;
};

}  // namespace infra::fixture

#endif  // ITMO_PARSAT_TESTINGCONFIGURATION_H
