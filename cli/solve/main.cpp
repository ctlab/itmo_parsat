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
#include "core/solve/Solve.h"
#include "core/sat/solver/sequential/Solver.h"

struct {
  int verbose;
  std::filesystem::path log_config;
  std::filesystem::path solve_config;
  std::filesystem::path input_cnf;
  std::filesystem::path model_path;
  bool print_model;
} config;

util::CliConfig add_and_read_args(int argc, char** argv) {
  namespace po = boost::program_options;
  po::options_description options("SAT solver CLI tool");
  options.add_options()                                                                                //
      ("verbose,v", po::value(&config.verbose)->default_value(2), "Verbosity level.")                  //
      ("log-config,l", po::value(&config.log_config)->required(), "Path to logging configuration.")    //
      ("solve-config,e", po::value(&config.solve_config)->required(), "Path to solve configuration.")  //
      ("input,i", po::value(&config.input_cnf)->required(), "Input file with dimacs-CNF formula.")     //
      ("print-model,p", po::bool_switch(&config.print_model)->default_value(false),
       "Whether to print satisfying model")(
          "model-path", po::value(&config.model_path), "Optional path to save sat certificate to");

  util::CliConfig cli_config;
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
    util::CliConfig::read_config(ifs, solve_config);
  }
  {
    IPS_INFO("Log config: " << log_config_path);
    std::ifstream ifs(log_config_path);
    util::CliConfig::read_config(ifs, log_config);
  }
  return {solve_config, log_config};
}

void do_print_model(std::ostream& os, Mini::vec<Mini::lbool> const& model) {
  os << "v ";
  for (int var = 0; var < model.size(); ++var) {
    os << (model[var] == Mini::l_True ? var + 1 : -var - 1) << ' ';
  }
  os << "0\n";
}

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  LOG(INFO) << std::fixed << std::setprecision(5);
  util::CliConfig cli_config = add_and_read_args(argc, argv);
  util::signal::SigHandler sig_handler;
  fLI::FLAGS_v = config.verbose;
  IPS_INFO("Input file: " << config.input_cnf);
  core::sat::Problem problem(config.input_cnf);
  core::sat::State result = problem.get_result();
  Mini::vec<Mini::lbool> sat_model;
  if (result == core::sat::UNKNOWN) {
    auto&& [solve_config, log_config] = read_json_configs(config.solve_config, config.log_config);
    util::random::Generator generator(solve_config.random_seed());
    util::Logger::set_logger_config(log_config);
    core::solve::RSolve solve(core::solve::SolveRegistry::resolve(solve_config));
    util::event::EventCallbackHandle solve_interrupt_handler = util::event::attach(
        [&] {
          IPS_INFO("Solve has been interrupted.");
          solve->interrupt();
          solve_interrupt_handler->detach();
        },
        util::event::INTERRUPT);
    result = IPS_BLOCK_R(solve_solve, solve->solve(problem));
    solve_interrupt_handler->detach();

    if (config.print_model && result == core::sat::SAT) {
      sat_model = problem.remap_and_extend_model(solve->get_model());
    }
  } else {
    if (config.print_model && result == core::sat::SAT) {
      // No need to remap variables because model is from
      // Minisat instance which has been used to eliminate
      sat_model = problem.get_model();
    }
  }

  IPS_TRACE_SUMMARY;
  if (result == core::sat::UNSAT) {
    std::cout << "s UNSATISFIABLE" << std::endl;
    return 0;
  } else if (result == core::sat::SAT) {
    std::cout << "s SATISFIABLE" << std::endl;
    if (config.print_model) {
      if (config.model_path.empty()) {
        do_print_model(std::cout, sat_model);
      } else {
        std::ofstream ofs(config.model_path);
        do_print_model(ofs, sat_model);
      }
    }
    return 1;
  } else {
    std::cout << "s UNKNOWN" << std::endl;
    return 2;
  }
}
