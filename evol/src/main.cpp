#include <glog/logging.h>

#include <boost/program_options.hpp>
#include <boost/timer/progress_display.hpp>
#include <fstream>
#include <mutex>

#include "evol/include/algorithm/Algorithm.h"
#include "evol/include/domain/Instance.h"
#include "evol/include/config/CliConfig.h"
#include "evol/include/config/Configuration.h"
#include "evol/include/sat/Solver.h"
#include "evol/include/util/Registry.h"
#include "evol/include/util/stream.h"
#include "evol/include/util/SigHandler.h"
#include "evol/include/util/Logging.h"
#include "evol/proto/config.pb.h"

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
  ::google::InitGoogleLogging(argv[0]);
  ea::config::CliConfig& config = add_and_read_args(argc, argv);
  bool backdoor = config.has("backdoor");
  std::filesystem::path input = config.get<std::filesystem::path>("input");
  std::filesystem::path config_path = config.get<std::filesystem::path>("config");
  ea::sat::State result;

  read_json_configs(config_path);
  ea::Logger logger(argv[0], ea::config::global_config().logger_config());

  LOG(INFO) << "Input file: " << input;

  if (backdoor) {
    ea::algorithm::RAlgorithm algorithm(
        ea::registry::Registry::resolve_algorithm(ea::config::global_config().algorithm_type()));
    auto& solver = algorithm->get_solver();
    LOG_TIME(solver.parse_cnf(input));
    algorithm->prepare();

    ea::SigHandler sig_handler;
    ea::SigHandler::CallbackHandle alg_int_handle =
        sig_handler.register_callback([&algorithm](int) {
          algorithm->interrupt();
          LOG(INFO) << "Algorithm has been interrupted.";
        });

    LOG_TIME(algorithm->process());
    auto& r_backdoor = algorithm->get_best();
    LOG(INFO) << "Number of points visited: " << ea::domain::Instance::inaccurate_points();
    LOG(INFO) << "The best backdoor is: " << r_backdoor;

    alg_int_handle.remove();
    sig_handler.unset();

    bool interrupted = false;
    ea::SigHandler::CallbackHandle slv_int_handle =
        sig_handler.register_callback([&solver, &interrupted](int) {
          solver.interrupt();
          interrupted = true;
          LOG(INFO) << "Solver has been interrupted.";
        });

    std::vector<bool> vars = r_backdoor.get_vars().get_mask();
    ea::domain::UAssignment assignment_p =
        ea::domain::createFullSearch(ea::domain::Instance::var_map(), vars);

    std::atomic_bool satisfied = false, unknown = false;
    size_t num_vars = r_backdoor.fitness().pow_r;

    std::mutex progress_lock;
    boost::timer::progress_display progress((unsigned long) std::pow(2UL, num_vars), std::cerr);

    // clang-format off
    std::atomic_uint64_t propagated{0};
    std::atomic_uint64_t total{0};
    solver.solve_assignments(
        std::move(assignment_p), [&propagated, &total, &sig_handler, &satisfied, &unknown, &progress, &progress_lock](
          ea::sat::State result, bool prop_conflict, auto) {
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
              case ea::sat::UNSAT:
                return true;
              case ea::sat::UNKNOWN:
                unknown = true;
                return true;
              case ea::sat::SAT:
                satisfied = true;
              default:
                return false;
            }
        });
    // clang-format on
    LOG(INFO) << "Prop: " << propagated << ", total: " << total;
    LOG(INFO) << "Actual rho is: " << (double) propagated / (double) total;

    if (satisfied) {
      result = ea::sat::SAT;
    } else if (!unknown && !interrupted) {
      result = ea::sat::UNSAT;
    } else {
      result = ea::sat::UNKNOWN;
    }
    slv_int_handle.remove();
  } else {
    ea::sat::RSolver solver =
        ea::registry::Registry::resolve_solver(ea::config::global_config().solver_type());
    ea::SigHandler sig_handler;
    ea::SigHandler::CallbackHandle slv_int_handle = sig_handler.register_callback([&solver](int) {
      solver->interrupt();
      LOG(INFO) << "Solver has been interrupted.";
    });

    LOG_TIME(result = solver->solve_limited());
    slv_int_handle.remove();
  }

  if (result == ea::sat::UNSAT) {
    LOG(INFO) << "UNSAT";
    std::cout << "UNSAT";
    return 0;
  } else if (result == ea::sat::SAT) {
    LOG(INFO) << "SAT";
    std::cout << "SAT";
    return 1;
  } else {
    LOG(INFO) << "UNKNOWN";
    std::cout << "UNKNOWN";
    return 2;
  }
}
