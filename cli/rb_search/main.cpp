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
#include "core/evol/algorithm/Algorithm.h"
#include "core/tests/common/get.h"

struct {
  int verbose;
  int seed;
  std::filesystem::path log_config;
  std::filesystem::path algorithm_config;
  std::filesystem::path input_cnf;
  std::filesystem::path stats_path;
  std::filesystem::path rb_path;
  std::filesystem::path heuristic_path;
  bool print_model;
} config;

util::CliConfig add_and_read_args(int argc, char** argv) {
  namespace po = boost::program_options;
  po::options_description options("Rho-backdor search utility");
  options.add_options()                                                                                         //
      ("verbose,v", po::value(&config.verbose)->default_value(2), "Verbosity level.")                           //
      ("seed", po::value(&config.seed)->default_value(239), "Random seed")                                      //
      ("log-config,l", po::value(&config.log_config)->required(), "Path to logging configuration.")             //
      ("algorithm-config,c", po::value(&config.algorithm_config)->required(), "Path to search configuration.")  //
      ("stats-path", po::value(&config.stats_path), "Path where to save statistics")                            //
      ("rb-path", po::value(&config.rb_path), "Path where to save backdoor")                                    //
      ("heuristic-path", po::value(&config.heuristic_path), "Path where to save the result of heuristic")       //
      ("input,i", po::value(&config.input_cnf)->required(), "Input file with dimacs-CNF formula.");

  util::CliConfig cli_config;
  cli_config.add_options(options);
  if (!cli_config.parse(argc, argv)) {
    std::exit(0);
  }

  cli_config.notify();
  return cli_config;
}

std::pair<AlgorithmConfig, LoggingConfig> read_json_configs(
    std::filesystem::path const& algorithm_config_path, std::filesystem::path const& log_config_path) {
  AlgorithmConfig algorithm_config;
  LoggingConfig log_config;
  {
    IPS_INFO("Algorithm config: " << algorithm_config_path);
    std::ifstream ifs(algorithm_config_path);
    util::CliConfig::read_config(ifs, algorithm_config);
  }
  {
    IPS_INFO("Log config: " << log_config_path);
    std::ifstream ifs(log_config_path);
    util::CliConfig::read_config(ifs, log_config);
  }
  return {algorithm_config, log_config};
}

template <typename T>
void dump_vec(std::ofstream& ofs, std::vector<T> const& vec) {
  for (int v : vec) {
    ofs << v << ' ';
  }
  ofs << '\n';
}

template <typename T>
void dump_csv(std::ofstream& ofs, std::vector<T> const& vec) {
  for (size_t i = 0; i < vec.size(); ++i) {
    ofs << i << ',' << vec[i] << '\n';
  }
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

  if (result == core::sat::UNKNOWN) {
    auto&& [alg_config, log_config] = read_json_configs(config.algorithm_config, config.log_config);
    util::Logger::set_logger_config(log_config);
    util::random::Generator generator(config.seed);

    auto rprop = common::get_prop(common::configs_path + "par_prop.json");
    rprop->load_problem(problem);
    auto preprocess = common::get_preprocess(rprop);

    if (!IPS_BLOCK_R(algorithm_preprocess, preprocess->preprocess(problem))) {
      IPS_INFO("Preprocessing returned false, aborting.");
    } else {
      if (!config.heuristic_path.empty()) {
        IPS_INFO("Saving heuristic results");
        std::ofstream ofs(config.heuristic_path);
        dump_vec(ofs, problem.remap_variables(preprocess->var_view().get_map()));
      }

      ea::algorithm::RAlgorithm algorithm(ea::algorithm::AlgorithmRegistry::resolve(alg_config, rprop));
      IPS_BLOCK(algorithm_prepare, algorithm->prepare(preprocess));
      util::event::EventCallbackHandle solve_interrupt_handler = util::event::attach(
          [&] {
            IPS_INFO("RB search has been interrupted.");
            algorithm->interrupt();
            solve_interrupt_handler->detach();
          },
          util::event::INTERRUPT);
      IPS_BLOCK(algorithm_process, algorithm->process());
      solve_interrupt_handler->detach();

      IPS_INFO("Result: " << algorithm->get_best());
      if (!config.stats_path.empty()) {
        IPS_INFO("Dumping statistics to " << config.stats_path);
        auto const& stats = algorithm->get_statistics();
        std::ofstream ofs(config.stats_path);
        ofs << "Duration: " << std::setprecision(4) << static_cast<float>(stats.duration_ms) / 1000.f << '\n';
        dump_csv(ofs, stats.rho_value);
      }
      if (!config.rb_path.empty()) {
        IPS_INFO("Dumping backdoor to " << config.rb_path);
        std::ofstream ofs(config.rb_path);
        dump_vec(ofs, problem.remap_variables(algorithm->get_best().get_vars().map_to_vars(preprocess->var_view())));
      }
    }
  } else {
    IPS_INFO("Problem is solved on parsing stage, aborting.");
  }

  IPS_TRACE_SUMMARY;
  return 0;
}
