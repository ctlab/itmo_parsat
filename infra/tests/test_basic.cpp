#include "infra/testing/LaunchFixture.h"

#include <filesystem>

TEST_F(LaunchFixture, base_test_no_backdoor) {
  infra::testing::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("naive.json")
      .set_description("Naive solver tests (no RBS)");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
}

TEST_F(LaunchFixture, basic_rbs_test) {
  infra::testing::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("rbs.json")
      .set_description("RBSSearch solver tests (RBS)");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
}

TEST_F(LaunchFixture, basic_par_rbs_test) {
  infra::testing::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("par_rbs.json")
      .set_description("ParRBSSearch solver tests (parallel RBS with CP solve)");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
}
