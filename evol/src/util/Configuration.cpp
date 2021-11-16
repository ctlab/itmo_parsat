#include "evol/include/util/Configuration.h"

#include <glog/logging.h>

namespace ea {

namespace po = boost::program_options;

Configuration::Configuration() {
  desc_.add_options()("help,h", po::bool_switch()->default_value(false), "Display help message");
}

void Configuration::add_options(const po::options_description& options) {
  desc_.add(options);
}

void Configuration::parse(int argc, char** argv) {
  po::store(po::parse_command_line(argc, argv, desc_), vm_);

  if (vm_["help"].as<bool>()) {
    LOG(FATAL) << desc_ << std::endl;
  }
}

void Configuration::parse(std::filesystem::path const& path) {
  po::store(po::parse_config_file(path.c_str(), desc_), vm_);
}

Configuration& Configuration::instance() {
  static Configuration config_;
  return config_;
}

}  // namespace ea