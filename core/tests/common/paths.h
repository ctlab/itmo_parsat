#ifndef ITMO_PARSAT_PATHS_H
#define ITMO_PARSAT_PATHS_H

#include "util/Random.h"
#include "core/sat/native/mini/mtl/Vec.h"
#include "core/sat/native/mini/utils/Lit.h"

#include "core/evol/algorithm/Algorithm.h"
#include "core/tests/common/paths.h"
#include "util/CliConfig.h"
#include "util/mini.h"
#include "util/stream.h"

namespace common {

static const std::string configs_path =
    IPS_PROJECT_ROOT "/core/tests/resources/";
static const std::string preprocess_config_path =
    configs_path + "preprocess.json";
static const std::string log_config_path = configs_path + "log.json";

#define MAX_INPUT_LEN 128
#define N_INPUTS (sizeof(common::inputs) / MAX_INPUT_LEN)
#define FS_PATH_INPUT(INPUT)                                   \
  std::filesystem::path INPUT(                                 \
      std::string(IPS_PROJECT_ROOT "/resources/cnf/common/") + \
      common::inputs[state.range(1)])

extern char const inputs[1][MAX_INPUT_LEN];

std::vector<int> gen_vars(size_t size, int num_vars);

Mini::vec<Mini::Lit> to_mini(std::vector<int> const& vars);

void set_logger_config();

core::sat::prop::RProp get_prop(std::filesystem::path const& config_path);

ea::preprocess::RPreprocess get_preprocess();

ea::algorithm::RAlgorithm get_algorithm(
    core::sat::prop::RProp rprop, std::filesystem::path const& config_path);

void load_problem(Minisat::SimpSolver& solver, core::sat::Problem const& problem);

}  // namespace common

#endif  // ITMO_PARSAT_PATHS_H
