#include "core/util/CliConfig.h"

namespace core {

namespace po = boost::program_options;

CliConfig::CliConfig() {
  desc_.add_options()("help,h", po::bool_switch()->default_value(false), "Display help message");
}

void CliConfig::add_options(const po::options_description& options) {
  desc_.add(options);
}

void CliConfig::parse(int argc, char** argv) {
  po::store(po::parse_command_line(argc, argv, desc_), vm_);

  if (vm_["help"].as<bool>()) {
    std::cerr << desc_ << std::endl;
    std::exit(-1);
  }
}

void CliConfig::parse(std::filesystem::path const& path) {
  po::store(po::parse_config_file(path.c_str(), desc_), vm_);
}

void CliConfig::notify() {
  po::notify(vm_);
}

bool CliConfig::has(const std::string& name) const {
  return vm_.count(name) > 0;
}

CliConfig& CliConfig::instance() {
  static CliConfig config_;
  return config_;
}

void CliConfig::read_config(std::istream& is, google::protobuf::Message& message) {
  std::string message_str((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
  google::protobuf::util::Status status =
      google::protobuf::util::JsonStringToMessage(message_str, &message);
  if (!status.ok()) {
    std::cerr << "Failed to read  configuration from '" << message_str << "':\n"
              << status.error_message();
    IPS_TERMINATE();
  }
}

}  // namespace core