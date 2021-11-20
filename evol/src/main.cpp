#include <glog/logging.h>

#include <boost/program_options.hpp>
#include <fstream>

#include "evol/include/algorithm/BaseAlgorithm.h"
#include "evol/include/config/Configuration.h"
#include "evol/include/sat/Solver.h"
#include "evol/include/util/CliConfig.h"
#include "evol/include/domain/RBDInstance.h"
#include "evol/include/util/Registry.h"

ea::CliConfig& add_and_read_args(int argc, char** argv) {
  namespace po = boost::program_options;

  po::options_description description;
  description.add_options()
      ("backdoor,b", "Enable rho-backdoor search.")
      ("ea-config,e", po::value<std::filesystem::path>(), "Path to JSON configuration for backdoor search EA algorithm.")
      ("minisat-config,m", po::value<std::filesystem::path>()->required(), "Path to minisat and solvers configuration.")
      ("input,i", po::value<std::filesystem::path>()->required(), "Input file with CNF formula.");

  ea::CliConfig& cli_config = ea::CliConfig::instance();
  cli_config.add_options(description);
  cli_config.parse(argc, argv);
  cli_config.notify();
  /* TODO: add configuration file parsing either here or in CliConfig */

  return cli_config;
}

void read_json_configs(
    std::filesystem::path const& global_cfg_path,
    std::filesystem::path const& minisat_cfg_path
) {
  using namespace ea::config;
  if (!global_cfg_path.empty()) {
    std::ifstream ifs(global_cfg_path);
    Configuration::read(Configuration::get_global_config(), ifs);
  }

  if (!minisat_cfg_path.empty()) {
    std::ifstream ifs(minisat_cfg_path);
    Configuration::read(Configuration::get_minisat_config(), ifs);
  }
}

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  ea::CliConfig& config = add_and_read_args(argc, argv);

  bool backdoor = config.has("backdoor");
  std::filesystem::path input = config.get<std::filesystem::path>("input");
  std::filesystem::path minisat_cfg = config.get<std::filesystem::path>("minisat-config");
  std::filesystem::path ea_cfg;
  ea::sat::State result = ea::sat::State::UNKNOWN;

  if (backdoor) {
    LOG_IF(FATAL, !config.has("ea-config")) << "EA config is not specified when backdoor search is enabled.";
    ea_cfg = config.get<std::filesystem::path>("ea-config");
  }
  read_json_configs(ea_cfg, minisat_cfg);
  LOG(INFO) << "Input file: " << input;

  if (backdoor) {
    /* Initialize population. TODO: heuristics */
    ea::instance::RPopulation population(new ea::instance::Population);
    population->push_back(
      ea::registry::Registry::instance().resolve_instance("RBDInstance")
    );

    /* Read configuration and start algorithm. */
    ea::algorithm::BaseAlgorithm algorithm;
    algorithm.set_population(population);
    algorithm.process();

    LOG(WARNING) << "Solving with backdoor is not yet implemented. "
                    "Proceeding with non-backdoor solution.";

    /* TODO: get backdoor and use it in solver. */
    ea::sat::Solver solver(ea::config::Configuration::get_minisat_config());
    solver.parse_cnf(input);
    solver.solve_limited();
    result = solver.state();
  } else {
    /* Solving without backdoor algorithm. */
    ea::sat::Solver solver(ea::config::Configuration::get_minisat_config());
    solver.parse_cnf(input);
    solver.solve_limited();
    result = solver.state();
  }

  if (result == ea::sat::UNSAT) {
    LOG(INFO) << "Result: UNSAT";
  } else if (result == ea::sat::SAT) {
    LOG(INFO) << "Result: SAT";
  } else {
    LOG(INFO) << "Result: UNDEFINED";
  }

  return 0;
}
