#include <glog/logging.h>

#include <boost/program_options.hpp>
#include <boost/timer/progress_display.hpp>
#include <fstream>

#include "evol/include/algorithm/Algorithm.h"
#include "evol/include/config/CliConfig.h"
#include "evol/include/config/Configuration.h"
#include "evol/include/domain/Instance.h"
#include "evol/include/sat/Solver.h"
#include "evol/include/util/Registry.h"
#include "evol/include/util/SigHandler.h"

ea::config::CliConfig& add_and_read_args(int argc, char** argv) {
  namespace po = boost::program_options;

  po::options_description description;
  // clang-format off
  description.add_options()
      ("backdoor,b", "Enable rho-backdoor search.")
      ("config,c", po::value<std::filesystem::path>(), "Path to JSON configuration.")
      ("input,i", po::value<std::filesystem::path>()->required(), "Input file with CNF formula.");
  // clang-format on

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

  LOG_TIME(4, read_json_configs(config_path));
  VLOG(3) << "Input file: " << input;

  std::shared_ptr<ea::sat::Solver> solver = ea::registry::Registry::resolve_solver(
      ea::config::Configuration::get_global_config().solver_type());
  LOG_TIME(4, solver->parse_cnf(input));

  if (backdoor) {
    ea::algorithm::RAlgorithm algorithm(
        ea::registry::Registry::resolve_algorithm(ea::config::global_config().algorithm_type()));
    algorithm->get_solver().parse_cnf(input);
    algorithm->prepare();

    ea::SigHandler::CallbackHandle alg_int_handle =
        ea::SigHandler::register_callback([&algorithm](int) {
          algorithm->interrupt();
          VLOG(2) << "Algorithm has been interrupted.";
        });

    LOG_TIME(4, algorithm->process());
    auto& r_backdoor = algorithm->get_best();
    VLOG(3) << "Resulting instance rho: " << std::fixed << r_backdoor.fitness().rho
            << ", size: " << r_backdoor.fitness().pow_r;

    alg_int_handle.remove();
    ea::SigHandler::unset();
    bool interrupted = false;
    ea::SigHandler::CallbackHandle slv_int_handle =
        ea::SigHandler::register_callback([&solver, &interrupted](int) {
          solver->interrupt();
          interrupted = true;
          VLOG(2) << "Solver has been interrupted.";
        });

    std::vector<bool> vars = r_backdoor.get_variables();
    ea::instance::FullSearch assignment(vars);

    bool satisfied = false, unknown = false;
#define DISPLAY_PROGRESS
#ifdef DISPLAY_PROGRESS
    size_t num_vars = r_backdoor.fitness().pow_r;
    boost::timer::progress_display progress((unsigned long) std::pow(2UL, num_vars));
#endif
    do {
      switch (solver->solve_limited(assignment())) {
        case ea::sat::UNSAT:
          break;
        case ea::sat::UNKNOWN:
          unknown = true;
          break;
        case ea::sat::SAT:
          satisfied = true;
          break;
      }
#ifdef DISPLAY_PROGRESS
      ++progress;
#endif
    } while (!ea::SigHandler::is_set() && !satisfied && ++assignment);

    if (satisfied) {
      result = ea::sat::SAT;
    } else if (!unknown && !interrupted) {
      result = ea::sat::UNSAT;
    } else {
      result = ea::sat::UNKNOWN;
    }
    slv_int_handle.remove();
  } else {
    ea::SigHandler::CallbackHandle slv_int_handle =
        ea::SigHandler::register_callback([&solver](int) {
          solver->interrupt();
          VLOG(2) << "Solver has been interrupted.";
        });

    LOG_TIME(4, result = solver->solve_limited());
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
