#ifndef EVOL_CLICONFIG_H
#define EVOL_CONFIGURATION_H

#include <iostream>

#include "evol/proto/config.pb.h"

namespace ea::config {

class Configuration {
 public:
  /**
   * Reads GlobalConfig from input stream
   */
  Configuration() = default;

  void read(std::istream& is);

  static Configuration& instance();

  static GlobalConfig& get_config();

 private:
  GlobalConfig gc_{};
};

}  // namespace ea::config

#endif  // EVOL_CLICONFIG_H
