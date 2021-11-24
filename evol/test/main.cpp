#include <glog/logging.h>
#include <gtest/gtest.h>

#include <fstream>

#include "evol/include/config/Configuration.h"

void read_config() {
  std::ifstream is_config("resources/config/ea/base.json");
  ea::config::read(ea::config::global_config(), is_config);
}

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  read_config();

  ::testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}
