#include "infra/testing/LaunchFixture.h"

#include <filesystem>

TEST_F(LaunchFixture, simp_test) {
  infra::domain::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("simp.json")
      .set_description("SimpSolver solver tests");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
  ASSERT_FALSE(test_failed);
}

TEST_F(LaunchFixture, maple_test) {
  infra::domain::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("maple.json")
      .set_description("MapleCOMSPS solver tests");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
  ASSERT_FALSE(test_failed);
}

TEST_F(LaunchFixture, painless_test) {
  infra::domain::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("painless.json")
      .set_description("Painless solver tests");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
  ASSERT_FALSE(test_failed);
}
