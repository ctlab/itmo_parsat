#include <boost/program_options.hpp>
#include <mutex>
#include <utility>
#include <glog/logging.h>

#include "util/CliConfig.h"
#include "core/proto/solve_config.pb.h"
#include "util/stream.h"
#include "util/Logger.h"
#include "util/TimeTracer.h"
#include "util/Random.h"
#include "util/SigHandler.h"
#include "core/sat/solver/Solver.h"
#include "core/solve/Solve.h"

core::CliConfig add_and_read_args(int argc, char** argv) {
  namespace po = boost::program_options;
  po::options_description options;
  // clang-format off
  options.add_options()
      ("verbose,v", po::value<int>()->default_value(2), "Verbosity level.")
      ("log-config,l", po::value<std::filesystem::path>()->required(), "Path to JSON Logging configuration.")
      ("solve-config,e", po::value<std::filesystem::path>()->required(), "Path to JSON Solve configuration.")
      ("input,i", po::value<std::filesystem::path>()->required(), "Input file with CNF formula.");
  // clang-format on

  core::CliConfig cli_config;
  cli_config.add_options(options);
  if (!cli_config.parse(argc, argv)) {
    std::exit(0);
  }

  cli_config.notify();
  return cli_config;
}

std::pair<SolveConfig, LoggingConfig> read_json_configs(
    std::filesystem::path const& solve_config_path, std::filesystem::path const& log_config_path) {
  SolveConfig solve_config;
  LoggingConfig log_config;
  {
    IPS_INFO("Solve config: " << solve_config_path);
    std::ifstream ifs(solve_config_path);
    core::CliConfig::read_config(ifs, solve_config);
  }
  {
    IPS_INFO("Log config: " << log_config_path);
    std::ifstream ifs(log_config_path);
    core::CliConfig::read_config(ifs, log_config);
  }
  return {solve_config, log_config};
}

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  LOG(INFO) << std::fixed << std::setprecision(5);
  core::CliConfig config = add_and_read_args(argc, argv);

  core::signal::SigHandler sig_handler;
  if (config.has("verbose")) {
    fLI::FLAGS_v = config.get<int>("verbose");
  }
  std::filesystem::path input = config.get<std::filesystem::path>("input");
  std::filesystem::path solver_cfg_path = config.get<std::filesystem::path>("solve-config");
  std::filesystem::path logger_cfg_path = config.get<std::filesystem::path>("log-config");
  IPS_INFO("Input file: " << input);

  auto&& [solve_config, log_config] = read_json_configs(solver_cfg_path, logger_cfg_path);
  core::Logger::set_logger_config(log_config);

  // Load problem
  core::sat::Problem problem(input);

  // Initialize solve algorithm
  core::RSolve solve(core::SolveRegistry::resolve(solve_config));
  core::event::EventCallbackHandle solve_interrupt_handler = core::event::attach(
      [&] {
        IPS_INFO("Solve has been interrupted.");
        solve->interrupt();
        solve_interrupt_handler->detach();
      },
      core::event::INTERRUPT);

  util::random::Generator generator(solve_config.random_seed());
  core::sat::State result = IPS_TRACE_V(solve->solve(problem));
  core::TimeTracer::print_summary(10);

  if (result == core::sat::UNSAT) {
    IPS_INFO("UNSAT");
    std::cout << "UNSAT" << std::endl;
    return 0;
  } else if (result == core::sat::SAT) {
    IPS_INFO("SAT");
    std::cout << "SAT" << std::endl;
    return 1;
  } else {
    IPS_INFO("UNKNOWN");
    std::cout << "UNKNOWN" << std::endl;
    return 2;
  }
}
