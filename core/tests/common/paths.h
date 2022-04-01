#ifndef ITMO_PARSAT_PATHS_H
#define ITMO_PARSAT_PATHS_H

#include <string>
#include <vector>
#include <filesystem>
#include <mutex>
#include <unordered_map>

#include "core/sat/Problem.h"

namespace common {

static const std::string configs_path =
    IPS_PROJECT_ROOT "/core/tests/resources/";
static const std::string preprocess_config_path =
    configs_path + "preprocess.json";
static const std::string log_config_path = configs_path + "log.json";

#define FS_PATH_INPUT(INPUT, NAME) \
  std::filesystem::path INPUT(     \
      std::string(IPS_PROJECT_ROOT "/resources/cnf/") + (NAME))

#define FS_PROBLEM_INPUT(INPUT, NAME) \
  core::sat::Problem INPUT(           \
      std::string(IPS_PROJECT_ROOT "/resources/cnf/") + (NAME))

std::vector<std::string> const& inputs(std::string const& group);

}  // namespace common

#endif  // ITMO_PARSAT_PATHS_H
