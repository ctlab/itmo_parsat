#include <glog/logging.h>

#include <boost/program_options.hpp>
#include <boost/timer/progress_display.hpp>
#include <mutex>
#include <utility>

#include "cli/solve/CliConfig.h"
#include "core/proto/config.pb.h"
#include "core/util/stream.h"
#include "core/util/SigHandler.h"
#include "core/sat/Solver.h"
#include "evol/algorithm/Algorithm.h"
#include "evol/instance/Instance.h"

ea::config::CliConfig& add_and_read_args(int argc, char** argv) {
  namespace po = boost::program_options;

  po::options_description description;
  // clang-format off
  description.add_options()
      ("backdoor,b", "Enable rho-backdoor search.")
      ("config,e", po::value<std::filesystem::path>(), "Path to JSON Solve configuration.")
      ("input,i", po::value<std::filesystem::path>()->required(), "Input file with CNF formula.");
  // clang-format on

  ea::config::CliConfig& cli_config = ea::config::CliConfig::instance();
  cli_config.add_options(description);
  cli_config.parse(argc, argv);
  cli_config.notify();

  return cli_config;
}

SolveConfig read_json_configs(std::filesystem::path const& config_path) {
  using namespace ea::config;
  SolveConfig solve_config;
  std::ifstream ifs(config_path);
  core::sat::SolverRegistry::read_config(ifs, solve_config);
  return solve_config;
}

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  ea::config::CliConfig& config = add_and_read_args(argc, argv);
  bool backdoor = config.has("backdoor");
  std::filesystem::path input = config.get<std::filesystem::path>("input");
  std::filesystem::path solver_cfg_path = config.get<std::filesystem::path>("config");
  core::sat::State result;

  auto solve_config = read_json_configs(solver_cfg_path);

  LOG(INFO) << "Input file: " << input;
  core::sat::RSolver solver(core::sat::SolverRegistry::resolve(solve_config.solver_config()));
  solver->parse_cnf(input);
  //  LOG_TIME(solver->parse_cnf(input));

  if (backdoor) {
    ea::algorithm::RAlgorithm algorithm(
        ea::algorithm::AlgorithmRegistry::resolve(solve_config.algorithm_config()));
    auto& algorithm_solver = algorithm->get_solver();
    algorithm_solver.parse_cnf(input);
    //    LOG_TIME(algorithm_solver.parse_cnf(input));
    algorithm->prepare();

    core::SigHandler sig_handler;
    core::SigHandler::CallbackHandle alg_int_handle =
        sig_handler.register_callback([&alg_int_handle, &algorithm](int) {
          algorithm->interrupt();
          LOG(INFO) << "Algorithm has been interrupted.";
          alg_int_handle.remove();
        });

    algorithm->process();
    //    LOG_TIME(algorithm->process());
    auto& r_backdoor = algorithm->get_best();
    LOG(INFO) << "Number of points visited: " << algorithm->inaccurate_points();
    LOG(INFO) << "The best backdoor is: " << r_backdoor;

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
          LOG(INFO) << "Solver has been interrupted.";
          slv_int_handle.remove();
        });

    // clang-format off
    solver->solve_assignments(
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
        });
    sig_handler.unset();
    // clang-format on
    LOG(INFO) << "Prop: " << propagated << ", total: " << total;
    LOG(INFO) << "Actual rho is: " << (double) propagated / (double) total;

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
          LOG(INFO) << "Solver has been interrupted.";
          slv_int_handle.remove();
        });
    result = solver->solve_limited();
    //    LOG_TIME(result = solver->solve_limited());
    sig_handler.unset();
  }

  if (result == core::sat::UNSAT) {
    LOG(INFO) << "UNSAT";
    std::cout << "UNSAT";
    return 0;
  } else if (result == core::sat::SAT) {
    LOG(INFO) << "SAT";
    std::cout << "SAT";
    return 1;
  } else {
    LOG(INFO) << "UNKNOWN";
    std::cout << "UNKNOWN";
    return 2;
  }
}
