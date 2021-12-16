#include <boost/program_options.hpp>
#include <boost/timer/progress_display.hpp>
#include <mutex>
#include <utility>

#include "core/util/CliConfig.h"
#include "core/proto/solve_config.pb.h"
#include "core/util/stream.h"
#include "core/util/SigHandler.h"
#include "core/util/Logger.h"
#include "core/util/Tracer.h"
#include "core/sat/Solver.h"
#include "core/evol/algorithm/Algorithm.h"
#include "core/evol/instance/Instance.h"

core::CliConfig& add_and_read_args(int argc, char** argv) {
  namespace po = boost::program_options;

  po::options_description description;
  // clang-format off
  description.add_options()
      ("backdoor,b", "Enable rho-backdoor search.")
      ("log-config,l", po::value<std::filesystem::path>()->required(), "Path to JSON Logging configuration.")
      ("config,e", po::value<std::filesystem::path>()->required(), "Path to JSON Solve configuration.")
      ("input,i", po::value<std::filesystem::path>()->required(), "Input file with CNF formula.");
  // clang-format on

  core::CliConfig& cli_config = core::CliConfig::instance();
  cli_config.add_options(description);
  cli_config.parse(argc, argv);
  cli_config.notify();

  return cli_config;
}

std::pair<SolveConfig, LoggingConfig> read_json_configs(
    std::filesystem::path const& solve_config_path, std::filesystem::path const& log_config_path) {
  SolveConfig solve_config;
  LoggingConfig log_config;
  {
    std::ifstream ifs(solve_config_path);
    core::CliConfig::read_config(ifs, solve_config);
  }
  {
    std::ifstream ifs(log_config_path);
    core::CliConfig::read_config(ifs, log_config);
  }
  return {solve_config, log_config};
}

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);

  core::CliConfig& config = add_and_read_args(argc, argv);
  bool backdoor = config.has("backdoor");
  std::filesystem::path input = config.get<std::filesystem::path>("input");
  std::filesystem::path solver_cfg_path = config.get<std::filesystem::path>("config");
  std::filesystem::path logger_cfg_path = config.get<std::filesystem::path>("log-config");
  core::sat::State result;

  auto&& [solve_config, log_config] = read_json_configs(solver_cfg_path, logger_cfg_path);
  core::Logger logger(log_config);
  core::Tracer tracer;

  IPS_INFO("Input file: " << input);
  core::sat::RSolver solver(core::sat::SolverRegistry::resolve(solve_config.solver_config()));
  IPS_TRACE(solver->parse_cnf(input));

  if (backdoor) {
    ea::algorithm::RAlgorithm algorithm(
        ea::algorithm::AlgorithmRegistry::resolve(solve_config.algorithm_config()));
    auto& algorithm_solver = algorithm->get_solver();
    IPS_TRACE(algorithm_solver.parse_cnf(input));
    algorithm->prepare();

    core::SigHandler sig_handler;
    core::SigHandler::CallbackHandle alg_int_handle =
        sig_handler.register_callback([&alg_int_handle, &algorithm](int) {
          algorithm->interrupt();
          IPS_INFO("Algorithm has been interrupted.");
          alg_int_handle.remove();
        });

    IPS_TRACE(algorithm->process());
    auto& r_backdoor = algorithm->get_best();
    IPS_INFO("Number of points visited: " << algorithm->inaccurate_points());
    IPS_INFO("The best backdoor is: " << r_backdoor);

    std::atomic_bool satisfied = false, unknown = false;
    std::atomic_uint64_t propagated{0}, total{0};
    size_t num_vars = r_backdoor.fitness().pow_r;
    std::vector<bool> vars = r_backdoor.get_vars().get_mask();
    core::domain::UAssignment assignment_p =
        core::domain::createFullSearch(r_backdoor.var_view(), vars);
    std::mutex progress_lock;
    boost::timer::progress_display progress((unsigned long) std::pow(2UL, num_vars), std::cerr);

    bool interrupted = false;
    sig_handler.unset();
    core::SigHandler::CallbackHandle slv_int_handle =
        sig_handler.register_callback([&slv_int_handle, &solver, &interrupted](int) {
          solver->interrupt();
          interrupted = true;
          IPS_INFO("Solver has been interrupted.");
          slv_int_handle.remove();
        });

    // clang-format off
    IPS_TRACE_NAMED("solve_assignments", solver->solve_assignments(
        std::move(assignment_p), [&propagated, &total, &sig_handler, &satisfied, &unknown, &progress, &progress_lock](
          core::sat::State result, bool prop_conflict, auto) {
            if (sig_handler.is_set()) {
              return false;
            }
            if (prop_conflict) {
              ++propagated;
            }
            ++total;
            {
              std::lock_guard<std::mutex> lg(progress_lock);
              ++progress;
            }
            switch (result) {
              case core::sat::UNSAT:
                return true;
              case core::sat::UNKNOWN:
                unknown = true;
                return true;
              case core::sat::SAT:
                satisfied = true;
              default:
                return false;
            }
        }));
    sig_handler.unset();
    // clang-format on
    IPS_INFO("Prop: " << propagated << ", total: " << total);
    IPS_INFO("Actual rho is: " << (double) propagated / (double) total);

    if (satisfied) {
      result = core::sat::SAT;
    } else if (!unknown && !interrupted) {
      result = core::sat::UNSAT;
    } else {
      result = core::sat::UNKNOWN;
    }
  } else {
    core::SigHandler sig_handler;
    core::SigHandler::CallbackHandle slv_int_handle =
        sig_handler.register_callback([&slv_int_handle, &solver](int) {
          solver->interrupt();
          IPS_INFO("Solver has been interrupted.");
          slv_int_handle.remove();
        });
    IPS_TRACE(result = solver->solve_limited());
    sig_handler.unset();
  }

  if (result == core::sat::UNSAT) {
    IPS_INFO("UNSAT");
    std::cout << "UNSAT";
    return 0;
  } else if (result == core::sat::SAT) {
    IPS_INFO("SAT");
    std::cout << "SAT";
    return 1;
  } else {
    IPS_INFO("UNKNOWN");
    std::cout << "UNKNOWN";
    return 2;
  }
}
