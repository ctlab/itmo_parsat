#include <glog/logging.h>

#include <boost/program_options.hpp>
#include <boost/timer/progress_display.hpp>
#include <fstream>

#include "evol/include/algorithm/BaseAlgorithm.h"
#include "evol/include/config/CliConfig.h"
#include "evol/include/config/Configuration.h"
#include "evol/include/domain/RBDInstance.h"
#include "evol/include/sat/Solver.h"
#include "evol/include/util/Registry.h"
#include "evol/include/util/SigHandler.h"

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
  ea::sat::State result = ea::sat::UNKNOWN;

  LOG_TIME(read_json_configs(config_path));
  VLOG(3) << "Input file: " << input;

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

    ea::SigHandler::CallbackHandle alg_int_handle =
        ea::SigHandler::register_callback([&algorithm](int) {
          algorithm.interrupt();
          VLOG(2) << "Algorithm has been interrupted.";
        });

    algorithm.set_population(population);
    LOG_TIME(algorithm.process());
    VLOG(3) << "Resulting instance rho: " << std::fixed << population->front()->fitness().rho;

    alg_int_handle.remove();
    ea::SigHandler::unset();
    ea::SigHandler::CallbackHandle slv_int_handle =
        ea::SigHandler::register_callback([&solver](int) {
          solver->interrupt();
          VLOG(2) << "Solver has been interrupted.";
        });

    std::set<unsigned> vars = population->front()->get_variables();
    ea::instance::Assignment assignment(vars);

    bool satisfied = false, unknown = false;
    boost::timer::progress_display progress((int) std::pow(2, vars.size()));
    do {
      LOG_TIME(solver->solve_limited(assignment()));
      switch (solver->state()) {
        case ea::sat::UNSAT:
          break;
        case ea::sat::UNKNOWN:
          unknown = true;
          break;
        case ea::sat::SAT:
          satisfied = true;
          break;
      }
      ++progress;
    } while (!ea::SigHandler::is_set() && !satisfied && ++assignment);

    if (satisfied) {
      result = ea::sat::SAT;
    } else if (!unknown) {
      result = ea::sat::UNSAT;
    } else {
      // If solver failed to solve with assumptions, it will definitely
      // fail to solve it without any assumptions, so the result is unknown.
      result = ea::sat::UNKNOWN;
    }
    slv_int_handle.remove();
  } else {
    /* Solving without any assumptions. */
    ea::SigHandler::CallbackHandle slv_int_handle =
        ea::SigHandler::register_callback([&solver](int) {
          solver->interrupt();
          VLOG(2) << "Solver has been interrupted.";
        });

    LOG_TIME(solver->solve_limited());
    result = solver->state();
    slv_int_handle.remove();
  }

  if (result == ea::sat::UNSAT) {
    VLOG(1) << "UNSAT";
  } else if (result == ea::sat::SAT) {
    VLOG(1) << "SAT";
  } else {
    VLOG(1) << "UNKNOWN";
  }

  return 0;
}
