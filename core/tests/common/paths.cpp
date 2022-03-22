#include "core/tests/common/paths.h"

namespace common {

// clang-format off
char const inputs[1][MAX_INPUT_LEN] = {
    "unsat_bubble_vs_pancake_7_6_simp-@3.cnf",
//  "unsat_aim-100-1_6-no-1-@0.cnf",
//  "unsat_dubois22-@0.cnf",
//  "unsat_sgen6_900_100-@2.cnf",
//  "sat_par8-1-c-@1.cnf",
};
// clang-format on

std::vector<int> gen_vars(size_t size, int num_vars) {
  std::vector<int> vars(size);
  for (int& x : vars) {
    x = util::random::sample<int>(0, num_vars - 1);
  }
  return vars;
}

Mini::vec<Mini::Lit> to_mini(std::vector<int> const& vars) {
  Mini::vec<Mini::Lit> v(vars.size());
  for (size_t i = 0; i < vars.size(); ++i) {
    v[i] = Mini::mkLit(vars[i], false);
  }
  return v;
}

void set_logger_config() {
  LoggingConfig logging_config;
  util::CliConfig::read_config(common::log_config_path, logging_config);
  core::Logger::set_logger_config(logging_config);
}

core::sat::prop::RProp get_prop(std::filesystem::path const& config_path) {
  PropConfig prop_config;
  util::CliConfig::read_config(config_path, prop_config);
  return core::sat::prop::RProp(
      core::sat::prop::PropRegistry::resolve(prop_config));
}

ea::preprocess::RPreprocess get_preprocess() {
  PreprocessConfig preprocess_config;
  util::CliConfig::read_config(
      common::preprocess_config_path, preprocess_config);
  return std::make_shared<ea::preprocess::Preprocess>(preprocess_config);
}

ea::algorithm::RAlgorithm get_algorithm(
    core::sat::prop::RProp rprop, std::filesystem::path const& config_path) {
  AlgorithmConfig algorithm_config;
  util::CliConfig::read_config(config_path, algorithm_config);
  return ea::algorithm::RAlgorithm(
      ea::algorithm::AlgorithmRegistry::resolve(algorithm_config, rprop));
}

void load_problem(
    Minisat::SimpSolver& solver, core::sat::Problem const& problem) {
  {
    util::GzFile gz_file(problem.path());
    solver.parsing = true;
    Minisat::parse_DIMACS(gz_file.native_handle(), solver);
    solver.parsing = false;
    solver.eliminate(true);
  }
}

}  // namespace common