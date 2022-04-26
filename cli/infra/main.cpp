#include <iostream>
#include <filesystem>

#include <boost/program_options.hpp>

#include "util/CliConfig.h"
#include "infra/fixture/LaunchFixture.h"

util::CliConfig parse_args(int argc, char** argv, infra::fixture::TestingConfiguration& config) {
  namespace po = boost::program_options;
  po::options_description options("CLI testing utility");
  // clang-format off
  options.add_options()
    ("concurrency,j", po::value(&config.concurrency)->default_value(std::thread::hardware_concurrency()), "Max concurrency")
    ("time_limit", po::value(&config.time_limit)->default_value(UINT64_MAX), "Time limit in seconds")
    ("branch", po::value(&config.branch)->required(), "The current branch")
    ("commit", po::value(&config.commit)->required(), "The current commit")
    ("description", po::value(&config.description)->default_value(""), "The description of the test")
    ("pg_host", po::value(&config.pg_host)->default_value("localhost"), "The host running DB")
    ("unsat_only", po::bool_switch(&config.unsat_only)->default_value(false), "Test on only unsat formulas")
    ("fail_on_tle", po::bool_switch(&config.fail_on_tle)->default_value(false), "Whether to fail if TLE happens")
    ("repeat_each", po::value(&config.repeat_each)->default_value(1), "Repeat each test")
    ("num_tests", po::value(&config.num_tests)->default_value(0), "Number of tests (0 = unlimited)")
    ("allow_unspecified_size", po::bool_switch(&config.allow_unspecified_size)->default_value(false), "Allow tests of unspecified size")
    ("lookup", po::bool_switch(&config.lookup)->default_value(false), "Skip tests that are already done")
    ("size", po::value(&config.size)->default_value(0), "Tests size (the greater the larger)")
    ("save", po::bool_switch(&config.save)->default_value(false), "Whether to save results to DB")
    ("resources_dir,r", po::value(&config.resources_dir)->required(), "Directory with config/ and cnf/ subdirectories")
    ("working_dir,w", po::value(&config.working_dir)->required(), "Testing working dir")
    ("executable,e", po::value(&config.executable)->required(), "Executable path")
    ("test_groups", po::value(&config.test_groups)->multitoken()->default_value({}, "[]"), "Enabled test groups (directories in cnf/)")
    ("gtest_opts", po::value<std::vector<std::string>>()->multitoken(), "GTest options");
  // clang-format on

  util::CliConfig cli_config;
  cli_config.add_options(options);
  if (!cli_config.parse(argc, argv)) {
    std::exit(0);
  }

  cli_config.notify();
  return cli_config;
}

void init_googletest(char const* argv0, util::CliConfig const& config) {
  if (!config.has("gtest_opts")) {
    testing::InitGoogleTest();
  } else {
    auto const& gtest_options = config.get<std::vector<std::string>>("gtest_opts");
    int argc = (int) gtest_options.size() + 1;
    std::vector<char const*> argv;
    argv.push_back(argv0);
    for (auto const& s : gtest_options) {
      argv.push_back(s.data());
    }
    ::testing::InitGoogleTest(&argc, (char**) argv.data());
  }
}

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  {  // Prepare LaunchFixture
    util::CliConfig config = parse_args(argc, argv, LaunchFixture::config);
    init_googletest(argv[0], config);
  }
  return RUN_ALL_TESTS();
}
