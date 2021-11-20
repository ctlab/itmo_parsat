#include "evol/include/config/Configuration.h"

#include <glog/logging.h>
#include <google/protobuf/util/json_util.h>

namespace ea::config {

Configuration& Configuration::instance() {
  static Configuration config_;
  return config_;
}

GlobalConfig& Configuration::get_global_config() {
  return instance().gc_;
}

FullMinisatSolverConfig& Configuration::get_minisat_config() {
  return instance().fmsc_;
}

void Configuration::read(google::protobuf::Message& message, std::istream& is) {
  std::string message_str((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
  google::protobuf::util::Status status = google::protobuf::util::JsonStringToMessage(message_str, &message);
  CHECK(status.ok()) << "Failed to read_global configuration from '" <<  message_str << "':\n"
                     << status.error_message();
}

}  // namespace ea