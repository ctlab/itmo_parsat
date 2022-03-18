#include <iostream>
#include <filesystem>

#include <boost/program_options.hpp>

#include "util/CliConfig.h"
#include "infra/testing/LaunchFixture.h"

util::CliConfig parse_args(int argc, char** argv) {
  namespace po = boost::program_options;
  po::options_description options("CLI testing utility");
  // clang-format off
  options.add_options()
    ("concurrency,j", po::value<uint32_t>()->default_value(std::thread::hardware_concurrency()), "Max concurrency")
    ("time-limit", po::value<uint64_t>()->default_value(UINT64_MAX), "Time limit in seconds")
    ("branch", po::value<std::string>()->default_value("")->required(), "The current branch")
    ("commit", po::value<std::string>()->default_value("")->required(), "The current commit")
    ("pg-host", po::value<std::string>()->default_value("localhost"), "The host running DB")
    ("unsat-only", po::bool_switch()->default_value(false), "Test on only unsat formulas")
    ("repeat-each", po::value<int>()->default_value(1), "Repeat each test")
    ("allow-unspecified-size", po::bool_switch()->default_value(false), "Allow tests of unspecified size")
    ("lookup", po::bool_switch()->default_value(false), "Skip tests that are already done")
    ("size", po::value<int>()->default_value(0), "Tests size (from 0 to 2)")
    ("save", po::bool_switch()->default_value(false), "Whether to save results to DB")
    ("resources-dir,r", po::value<std::filesystem::path>()->required(), "Directory with config/ and cnf/ subdirectories")
    ("working-dir,w", po::value<std::filesystem::path>()->required(), "Testing working dir")
    ("exec,e", po::value<std::filesystem::path>()->required(), "Executable path")
    ("test-groups", po::value<std::vector<std::string>>()->multitoken()->default_value({}, "[]"), "Enabled test groups (directories in cnf/)")
    ("gtest-opts", po::value<std::vector<std::string>>()->multitoken(), "GTest options");
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
  if (!config.has("gtest-opts")) {
    testing::InitGoogleTest();
  } else {
    auto const& gtest_options =
        config.get<std::vector<std::string>>("gtest-opts");
    int argc = (int) gtest_options.size() + 1;
    std::vector<char const*> argv;
    argv.push_back(argv0);
    for (auto const& s : gtest_options) {
      argv.push_back(s.data());
    }
    testing::InitGoogleTest(&argc, (char**) argv.data());
  }
}

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);

  {  // Prepare LaunchFixture
    util::CliConfig config = parse_args(argc, argv);
    init_googletest(argv[0], config);
    auto& lf_config = LaunchFixture::config;
    lf_config.unsat_only = config.get<bool>("unsat-only");
    lf_config.max_threads = config.get<uint32_t>("concurrency");
    lf_config.time_limit_s = config.get<uint64_t>("time-limit");
    lf_config.pg_host = config.get<std::string>("pg-host");
    lf_config.test_groups = config.get<std::vector<std::string>>("test-groups");
    lf_config.allow_unspecified_size =
        config.get<bool>("allow-unspecified-size");
    lf_config.repeat = config.get<int>("repeat-each");
    lf_config.lookup = config.get<bool>("lookup");
    lf_config.save = config.get<bool>("save");
    lf_config.size = config.get<int>("size");
    lf_config.executable = config.get<std::filesystem::path>("exec");
    lf_config.branch = config.get<std::string>("branch");
    lf_config.commit = config.get<std::string>("commit");
    lf_config.resources_dir =
        config.get<std::filesystem::path>("resources-dir");
    lf_config.working_dir = config.get<std::filesystem::path>("working-dir");
  }
  return RUN_ALL_TESTS();
}
