#include <glog/logging.h>

#include <iostream>

#include "evol/include/util/CliConfig.h"

namespace po = boost::program_options;

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);

  return 0;
}
