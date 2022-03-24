#ifndef ITMO_PARSAT_PATHS_H
#define ITMO_PARSAT_PATHS_H

#include <string>
#include <filesystem>

namespace common {

static const std::string configs_path =
    IPS_PROJECT_ROOT "/core/tests/resources/";
static const std::string preprocess_config_path =
    configs_path + "preprocess.json";
static const std::string log_config_path = configs_path + "log.json";

#define MAX_INPUT_LEN 128
#define N_INPUTS (sizeof(common::inputs) / MAX_INPUT_LEN)
#define FS_PATH_INPUT(INPUT, NAME) \
  std::filesystem::path INPUT(     \
      std::string(IPS_PROJECT_ROOT "/resources/cnf/common/") + (NAME))

extern char const inputs[7][MAX_INPUT_LEN];

}  // namespace common

#endif  // ITMO_PARSAT_PATHS_H
