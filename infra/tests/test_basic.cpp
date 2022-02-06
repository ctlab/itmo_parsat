#include "infra/testing/LaunchFixture.h"

#include <filesystem>

TEST_F(LaunchFixture, naive_test) {
  // For now, ignore this test because it is too large
  infra::testing::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("naive.json")
      .set_description("Naive solver tests (no RBS)");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
  ASSERT_FALSE(test_failed);
}

TEST_F(LaunchFixture, simp_rbs_test) {
  infra::testing::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("simp_rbs.json")
      .set_threads_required(8)
      .set_description("RBSSearch solver tests (RBS, sequential solve)");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
  ASSERT_FALSE(test_failed);
}

TEST_F(LaunchFixture, simp_par_rbs_test) {
  infra::testing::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("simp_par_rbs.json")
      .set_threads_required(8)
      .set_description("ParRBSSearch solver tests (ParRBS, sequential solve)");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
  ASSERT_FALSE(test_failed);
}

TEST_F(LaunchFixture, par_rbs_test) {
  infra::testing::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("par_rbs.json")
      .set_threads_required(8)
      .set_description("RBSSearch solver tests (RBS, parallel solve)");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
  ASSERT_FALSE(test_failed);
}

TEST_F(LaunchFixture, par_par_rbs_test) {
  infra::testing::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("par_par_rbs.json")
      .set_threads_required(8)
      .set_description("ParRBSSearch solver tests (ParRBS, parallel solve)");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
  ASSERT_FALSE(test_failed);
}
