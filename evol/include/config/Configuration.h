#ifndef EVOL_CONFIGURATION_H
#define EVOL_CONFIGURATION_H

#include <iostream>

#include "evol/proto/config.pb.h"

namespace ea::config {

class Configuration {
 public:
  Configuration() = default;

  /**
   * Reads protobuf message from input stream.
   */
  static void read(google::protobuf::Message& message, std::istream& is);

  static Configuration& instance();

  static GlobalConfig& get_global_config();

 private:
  GlobalConfig gc_{};
};

void read(google::protobuf::Message& message, std::istream& is);

GlobalConfig& global_config();

}  // namespace ea::config

#endif  // EVOL_CONFIGURATION_H
