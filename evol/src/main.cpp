#include <glog/logging.h>

#include <boost/program_options.hpp>
#include <fstream>

#include "evol/include/algorithm/BaseAlgorithm.h"
#include "evol/include/config/CliConfig.h"
#include "evol/include/config/Configuration.h"
#include "evol/include/domain/RBDInstance.h"
#include "evol/include/sat/Solver.h"
#include "evol/include/util/Registry.h"

ea::config::CliConfig& add_and_read_args(int argc, char** argv) {
  namespace po = boost::program_options;

  po::options_description description;
  description.add_options()("backdoor,b", "Enable rho-backdoor search.")(
      "config,c", po::value<std::filesystem::path>(), "Path to JSON configuration.")(
      "input,i", po::value<std::filesystem::path>()->required(), "Input file with CNF formula.");

  ea::config::CliConfig& cli_config = ea::config::CliConfig::instance();
  cli_config.add_options(description);
  cli_config.parse(argc, argv);
  cli_config.notify();

  return cli_config;
}

void read_json_configs(std::filesystem::path const& global_cfg_path) {
  using namespace ea::config;
  std::ifstream ifs(global_cfg_path);
  Configuration::read(Configuration::get_global_config(), ifs);
}

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  ea::config::CliConfig& config = add_and_read_args(argc, argv);

  bool backdoor = config.has("backdoor");
  std::filesystem::path input = config.get<std::filesystem::path>("input");
  std::filesystem::path config_path = config.get<std::filesystem::path>("config");
  ea::sat::State result;

  LOG_TIME(read_json_configs(config_path));
  LOG(INFO) << "Input file: " << input;

  /* Create solver */
  std::shared_ptr<ea::sat::Solver> solver = ea::registry::Registry::resolve_solver(
      ea::config::Configuration::get_global_config().solver_type());
  LOG_TIME(solver->parse_cnf(input));

  if (backdoor) {
    ea::instance::RPopulation population(new ea::instance::Population);
    population->push_back(ea::registry::Registry::resolve_instance(
        ea::config::Configuration::get_global_config().instance_type()));
    population->front()->set_solver(solver);

    /* Read configuration and start algorithm. */
    ea::algorithm::BaseAlgorithm algorithm;
    algorithm.set_population(population);
    LOG_TIME(algorithm.process());
    LOG(INFO) << "Resulting instance rho: " << std::fixed << population->front()->rho();

    Minisat::vec<Minisat::Lit> assumptions;
    population->front()->get_assumptions(assumptions);

    LOG_TIME(solver->solve_limited(assumptions));
    result = solver->state();
  } else {
    /* Solving without any assumptions. */
    LOG_TIME(solver->solve_limited());
    result = solver->state();
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
