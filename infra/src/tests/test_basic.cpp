#include "infra/include/testing/LaunchFixture.h"

#include <filesystem>

TEST_F(LaunchFixture, base_test_no_backdoor) {
  infra::testing::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_config_path("base.json")
      .set_description("Basic SimpSolver test (no backdoor)");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
}

TEST_F(LaunchFixture, base_test_backdoor) {
  infra::testing::LaunchConfig config;
  config.set_backdoor(true)
      .set_expected_result(infra::domain::UNKNOWN)
      .set_config_path("base.json")
      .set_description("Basic EA test");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
}
