#include <iostream>
#include <filesystem>

#include <boost/program_options.hpp>

#include "util/CliConfig.h"
#include "infra/fixture/LaunchFixture.h"

util::CliConfig parse_args(int argc, char** argv) {
  namespace po = boost::program_options;
  po::options_description options("CLI testing utility");
  // clang-format off
  options.add_options()
    ("concurrency,j", po::value<uint32_t>()->default_value(std::thread::hardware_concurrency()), "Max concurrency")
    ("time_limit", po::value<uint64_t>()->default_value(UINT64_MAX), "Time limit in seconds")
    ("branch", po::value<std::string>()->required(), "The current branch")
    ("commit", po::value<std::string>()->required(), "The current commit")
    ("description", po::value<std::string>()->default_value(""), "The description of the test")
    ("pg_host", po::value<std::string>()->default_value("localhost"), "The host running DB")
    ("unsat_only", po::bool_switch()->default_value(false), "Test on only unsat formulas")
    ("fail_on_tle", po::bool_switch()->default_value(false), "Whether to fail if TLE happens")
    ("repeat_each", po::value<int>()->default_value(1), "Repeat each test")
    ("num_tests", po::value<int>()->default_value(0), "Number of tests (0 = unlimited)")
    ("allow_unspecified_size", po::bool_switch()->default_value(false), "Allow tests of unspecified size")
    ("lookup", po::bool_switch()->default_value(false), "Skip tests that are already done")
    ("size", po::value<int>()->default_value(0), "Tests size (the greater the larger)")
    ("save", po::bool_switch()->default_value(false), "Whether to save results to DB")
    ("resources_dir,r", po::value<std::filesystem::path>()->required(), "Directory with config/ and cnf/ subdirectories")
    ("working_dir,w", po::value<std::filesystem::path>()->required(), "Testing working dir")
    ("executable,e", po::value<std::filesystem::path>()->required(), "Executable path")
    ("test_groups", po::value<std::vector<std::string>>()->multitoken()->default_value({}, "[]"), "Enabled test groups (directories in cnf/)")
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
    auto const& gtest_options =
        config.get<std::vector<std::string>>("gtest_opts");
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
    util::CliConfig config = parse_args(argc, argv);
    init_googletest(argv[0], config);
    auto& lf_config = LaunchFixture::config;
#define CLI_CFG_ARG(TYPE, NAME) lf_config.NAME = config.get<TYPE>(#NAME)
    CLI_CFG_ARG(bool, unsat_only);
    CLI_CFG_ARG(bool, fail_on_tle);
    CLI_CFG_ARG(bool, allow_unspecified_size);
    CLI_CFG_ARG(bool, lookup);
    CLI_CFG_ARG(bool, save);
    CLI_CFG_ARG(int, repeat_each);
    CLI_CFG_ARG(int, size);
    CLI_CFG_ARG(int, num_tests);
    CLI_CFG_ARG(uint32_t, concurrency);
    CLI_CFG_ARG(uint64_t, time_limit);
    CLI_CFG_ARG(std::string, description);
    CLI_CFG_ARG(std::string, pg_host);
    CLI_CFG_ARG(std::string, branch);
    CLI_CFG_ARG(std::string, commit);
    CLI_CFG_ARG(std::filesystem::path, executable);
    CLI_CFG_ARG(std::filesystem::path, resources_dir);
    CLI_CFG_ARG(std::filesystem::path, working_dir);
    CLI_CFG_ARG(std::vector<std::string>, test_groups);
#undef CLI_CFG_ARG
  }
  return RUN_ALL_TESTS();
}
