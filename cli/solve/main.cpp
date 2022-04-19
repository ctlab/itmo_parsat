#include <boost/program_options.hpp>
#include <mutex>
#include <utility>
#include <glog/logging.h>

#include "util/CliConfig.h"
#include "core/proto/solve_config.pb.h"
#include "util/stream.h"
#include "util/Logger.h"
#include "util/Random.h"
#include "util/TimeTracer.h"
#include "util/SigHandler.h"
#include "core/sat/solver/sequential/Solver.h"
#include "core/solve/Solve.h"

util::CliConfig add_and_read_args(int argc, char** argv) {
  namespace po = boost::program_options;
  po::options_description options("SAT solver CLI tool");
  // clang-format off
  options.add_options()
      ("verbose,v", po::value<int>()->default_value(2), "Verbosity level.")
      ("log-config,l", po::value<std::filesystem::path>()->required(), "Path to logging configuration.")
      ("solve-config,e", po::value<std::filesystem::path>()->required(), "Path to solve configuration.")
      ("input,i", po::value<std::filesystem::path>()->required(), "Input file with dimacs-CNF formula.");
  // clang-format on

  util::CliConfig cli_config;
  cli_config.add_options(options);
  if (!cli_config.parse(argc, argv)) { std::exit(0); }

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
    util::CliConfig::read_config(ifs, solve_config);
  }
  {
    IPS_INFO("Log config: " << log_config_path);
    std::ifstream ifs(log_config_path);
    util::CliConfig::read_config(ifs, log_config);
  }
  return {solve_config, log_config};
}

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  LOG(INFO) << std::fixed << std::setprecision(5);
  util::CliConfig config = add_and_read_args(argc, argv);
  core::signal::SigHandler sig_handler;
  fLI::FLAGS_v = config.get<int>("verbose");
  std::filesystem::path input = config.get<std::filesystem::path>("input");
  IPS_INFO("Input file: " << input);
  core::sat::Problem problem(input);
  core::sat::State result = problem.get_result();

  if (result == core::sat::UNKNOWN) {
    auto&& [solve_config, log_config] = read_json_configs(
        config.get<std::filesystem::path>("solve-config"), config.get<std::filesystem::path>("log-config"));

    util::random::Generator generator(solve_config.random_seed());
    core::Logger::set_logger_config(log_config);
    core::solve::RSolve solve(core::solve::SolveRegistry::resolve(solve_config));
    core::event::EventCallbackHandle solve_interrupt_handler = core::event::attach(
        [&] {
          IPS_INFO("Solve has been interrupted.");
          solve->interrupt();
          solve_interrupt_handler->detach();
        },
        core::event::INTERRUPT);
    result = IPS_TRACE_V(solve->solve(problem));
    solve_interrupt_handler->detach();
  }

  IPS_TRACE_SUMMARY(10);
  if (result == core::sat::UNSAT) {
    std::cout << "UNSAT" << std::endl;
    return 0;
  } else if (result == core::sat::SAT) {
    std::cout << "SAT" << std::endl;
    return 1;
  } else {
    std::cout << "UNKNOWN" << std::endl;
    return 2;
  }
}
