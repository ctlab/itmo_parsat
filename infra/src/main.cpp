#include <iostream>

#include <pqxx/pqxx>
#include <glog/logging.h>
#include <boost/program_options.hpp>

#include "infra/include/testing/LaunchFixture.h"

boost::program_options::variables_map parse_args(int argc, char** argv) {
  namespace po = boost::program_options;
  po::options_description options("CLI testing utility");
  // clang-format off
  options.add_options()
    ("help,h", po::bool_switch()->default_value(false), "Display help message")
    ("resources-dir,i", po::value<std::filesystem::path>()->required(), "Directory with config/ and cnf/ subdirectories")
    ("working-dir,w", po::value<std::filesystem::path>()->required(), "Testing working dir")
    ("exec,e", po::value<std::filesystem::path>()->required(), "Executable path")
    ("dbname,d", po::value<std::string>()->required(), "PG database name")
    ("pass,p", po::value<std::string>()->default_value(""), "PG password")
    ("user,u", po::value<std::string>()->default_value(""), "PG user");
  // clang-format on

  po::variables_map args;
  po::store(po::parse_command_line(argc, argv, options), args);
  if (args["help"].as<bool>()) {
    LOG(FATAL) << std::endl << options;
  }
  po::notify(args);
  return args;
}

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  ::testing::InitGoogleTest();

  { // Prepare LaunchFixture
    auto args = parse_args(argc, argv);
    auto& config = LaunchFixture::config;
    config.executable = args["exec"].as<std::filesystem::path>();
    config.resources_dir = args["resources-dir"].as<std::filesystem::path>();
    config.working_dir = args["working-dir"].as<std::filesystem::path>();
    config.dbname = args["dbname"].as<std::string>();
    config.user = args["user"].as<std::string>();
    config.password = args["pass"].as<std::string>();
    LaunchFixture::prepare();
  }

  return RUN_ALL_TESTS();
}
