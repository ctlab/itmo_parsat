#include "evol/include/config/Configuration.h"

#include <glog/logging.h>
#include <google/protobuf/util/json_util.h>

namespace ea::config {

Configuration& Configuration::instance() {
  static Configuration config_;
  return config_;
}

GlobalConfig& Configuration::get_config() {
  return instance().gc_;
}

void Configuration::read(std::istream& is) {
  std::string message((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
  google::protobuf::util::Status status = google::protobuf::util::JsonStringToMessage(message, &gc_);
  CHECK(status.ok()) << "Failed to read configuration from '" <<  message << "':"
                     << status.error_message();
}

}  // namespace ea