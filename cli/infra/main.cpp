#include <iostream>
#include <filesystem>

#include <boost/program_options.hpp>

#include "infra/testing/LaunchFixture.h"

boost::program_options::variables_map parse_args(int argc, char** argv) {
  namespace po = boost::program_options;
  po::options_description options("CLI testing utility");
  // clang-format off
  options.add_options()
    ("help,h", po::bool_switch()->default_value(false), "Display help message")
    ("commit", po::value<std::string>()->default_value(""), "The current commit (optional)")
    ("lookup", po::bool_switch()->default_value(false), "Skip tests that are already done")
    ("size", po::value<int>()->default_value(0), "Tests size (from 0 to 2)")
    ("save", po::bool_switch()->default_value(false), "Whether to save results to DB")
    ("resources-dir,r", po::value<std::filesystem::path>()->required(), "Directory with config/ and cnf/ subdirectories")
    ("working-dir,w", po::value<std::filesystem::path>()->required(), "Testing working dir")
    ("exec,e", po::value<std::filesystem::path>()->required(), "Executable path")
    ("dbname,d", po::value<std::string>()->required(), "PG database name")
    ("pass,p", po::value<std::string>()->default_value(""), "PG password")
    ("user,u", po::value<std::string>()->default_value(""), "PG user")
    ("gtest-opts", po::value<std::vector<std::string>>()->multitoken(), "GTest options");
  // clang-format on

  po::variables_map args;
  po::store(po::parse_command_line(argc, argv, options), args);
  if (args["help"].as<bool>()) {
    LOG(FATAL) << std::endl << options;
  }
  po::notify(args);
  return args;
}

void init_googletest(char const* argv0, boost::program_options::variables_map& vmap) {
  if (vmap.count("gtest-opts") == 0) {
    testing::InitGoogleTest();
  } else {
    auto const& gtest_options = vmap["gtest-opts"].as<std::vector<std::string>>();
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
    auto args = parse_args(argc, argv);
    init_googletest(argv[0], args);
    auto& config = LaunchFixture::config;
    config.lookup = args["lookup"].as<bool>();
    config.save = args["save"].as<bool>();
    config.size = args["size"].as<int>();
    config.executable = args["exec"].as<std::filesystem::path>();
    config.commit = args["commit"].as<std::string>();
    config.resources_dir = args["resources-dir"].as<std::filesystem::path>();
    config.working_dir = args["working-dir"].as<std::filesystem::path>();
    config.dbname = args["dbname"].as<std::string>();
    config.user = args["user"].as<std::string>();
    config.password = args["pass"].as<std::string>();
  }

  return RUN_ALL_TESTS();
}
