#include "core/tests/common/get.h"

namespace common {

void set_logger_config() {
  LoggingConfig logging_config;
  util::CliConfig::read_config(common::log_config_path, logging_config);
  util::Logger::set_logger_config(logging_config);
}

core::sat::prop::RProp get_prop(std::filesystem::path const& config_path) {
  PropConfig prop_config;
  util::CliConfig::read_config(config_path, prop_config);
  return core::sat::prop::RProp(core::sat::prop::PropRegistry::resolve(prop_config));
}

core::sat::solver::RSolver get_solver(std::filesystem::path const& config_path) {
  SolverConfig solver_config;
  util::CliConfig::read_config(config_path, solver_config);
  return core::sat::solver::RSolver(core::sat::solver::SolverRegistry::resolve(solver_config));
}

core::sat::solver::RSolverService get_solver_service(std::filesystem::path const& config_path) {
  SolverServiceConfig solver_service_config;
  util::CliConfig::read_config(config_path, solver_service_config);
  return core::sat::solver::RSolverService(core::sat::solver::SolverServiceRegistry::resolve(solver_service_config));
}

ea::preprocess::RPreprocess get_preprocess(core::sat::prop::RProp prop) {
  PreprocessConfig preprocess_config;
  util::CliConfig::read_config(common::preprocess_config_path, preprocess_config);
  return std::make_shared<ea::preprocess::Preprocess>(preprocess_config, std::move(prop));
}

ea::algorithm::RAlgorithm get_algorithm(core::sat::prop::RProp const& rprop, std::filesystem::path const& config_path) {
  AlgorithmConfig algorithm_config;
  util::CliConfig::read_config(config_path, algorithm_config);
  return ea::algorithm::RAlgorithm(ea::algorithm::AlgorithmRegistry::resolve(algorithm_config, rprop));
}

void load_problem(Minisat::SimpSolver& solver, core::sat::Problem const& problem) {
  solver.parsing = true;
  solver.loadClauses(problem.clauses());
  solver.parsing = false;
}

}  // namespace common
