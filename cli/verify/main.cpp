#include <boost/program_options.hpp>
#include <mutex>
#include <glog/logging.h>

#include "util/CliConfig.h"
#include "core/proto/solve_config.pb.h"
#include "util/stream.h"
#include "util/Logger.h"
#include "util/TimeTracer.h"
#include "core/sat/solver/sequential/Solver.h"

struct {
  std::filesystem::path input_cnf;
  std::filesystem::path certificate;
} config;

util::CliConfig add_and_read_args(int argc, char** argv) {
  namespace po = boost::program_options;
  po::options_description options("SAT certificate verification tool");
  options.add_options()                                                                //
      ("input,i", po::value(&config.input_cnf)->required(), "DIMACS cnf description")  //
      ("cert,c", po::value(&config.certificate)->required(), "DIMACS SAT certificate");

  util::CliConfig cli_config;
  cli_config.add_options(options);
  if (!cli_config.parse(argc, argv)) {
    std::exit(0);
  }

  cli_config.notify();
  return cli_config;
}

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  LOG(INFO) << std::fixed << std::setprecision(5);
  util::CliConfig cli_config = add_and_read_args(argc, argv);

  Minisat::SimpSolver solver;
  {
    util::GzFile formula(config.input_cnf);
    solver.parsing = true;
    Minisat::parse_DIMACS(formula.native_handle(), solver);
    solver.parsing = false;
  }

  Mini::vec<Mini::Lit> model;
  {
    std::unordered_set<int> used_vars;
    std::ifstream ifs(config.certificate);
    std::string token;
    while (ifs >> token) {
      if (token[0] == 'v' || token[0] == '0') {
        continue;
      }
      int v = std::atoi(token.c_str());
      int var = std::abs(v) - 1;
      IPS_VERIFY(used_vars.find(var) == used_vars.end());
      used_vars.insert(var);
      model.push(Mini::mkLit(var, v < 0));
      IPS_VERIFY(0 <= var && var < solver.nVars());
    }
    IPS_INFO("Specified " << model.size() << " variables");
    IPS_WARNING_IF(model.size() < solver.nVars(), "Not all variables have been specified");
    for (int i = 0; i < solver.nVars(); ++i) {
      if (used_vars.find(i) == used_vars.end()) {
        model.push(Mini::mkLit(i, false));
      }
    }
    IPS_INFO("Variables: " << solver.nVars() << ", model size: " << model.size());
  }

  if (!solver.prop_check(model)) {
    IPS_INFO("Incorrect SAT certificate");
    return 1;
  } else {
    IPS_INFO("SAT certificate check OK");
    return 0;
  }
}
