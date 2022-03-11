#include "CliConfig.h"

namespace util {

namespace po = boost::program_options;

CliConfig::CliConfig() {
  // clang-format off
  desc_.add_options()
      ("help,h", po::bool_switch()->default_value(false), "Display help message")
      ("config", po::value<std::filesystem::path>(), "Configuration file");
  // clang-format on
}

void CliConfig::add_options(const po::options_description& options) {
  desc_.add(options);
}

bool CliConfig::parse(int argc, char** argv) {
  po::store(po::parse_command_line(argc, argv, desc_), vm_);

  if (vm_["help"].as<bool>()) {
    std::cerr << desc_ << std::endl;
    return false;
  }

  if (vm_.count("config") > 0) {
    std::ifstream ifs(vm_["config"].as<std::filesystem::path>());
    po::store(po::parse_config_file(ifs, desc_), vm_);
    // Re-store options from command line in order to override
    po::store(po::parse_command_line(argc, argv, desc_), vm_);
  }
  return true;
}

void CliConfig::notify() {
  po::notify(vm_);
}

bool CliConfig::has(const std::string& name) const {
  return vm_.count(name) > 0;
}

void CliConfig::read_config(std::istream& is, google::protobuf::Message& message) {
  std::string message_str((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
  google::protobuf::util::Status status =
      google::protobuf::util::JsonStringToMessage(message_str, &message);
  if (!status.ok()) {
    IPS_VERIFY_S(
        status.ok(), "Failed to read  configuration from '" << message_str << "':\n"
                                                            << status.error_message());
  }
}

}  // namespace util