#include "infra/testing/LaunchFixture.h"

#include <filesystem>

constexpr uint32_t parallel = 16;

TEST_F(LaunchFixture, simp_test) {
  infra::testing::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("simp.json")
      .set_description("SimpSolver solver tests");
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
      .set_threads_required(parallel)
      .set_description("RBSSearch solver tests (RBS, SimpSolver sequential solve)");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
  ASSERT_FALSE(test_failed);
}

TEST_F(LaunchFixture, simp_parrbs_test) {
  infra::testing::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("simp_parrbs.json")
      .set_threads_required(parallel)
      .set_description("ParRBSSearch solver tests (ParRBS, SimpSolver sequential solve)");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
  ASSERT_FALSE(test_failed);
}

TEST_F(LaunchFixture, simppar_rbs_test) {
  infra::testing::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("simppar_rbs.json")
      .set_threads_required(parallel)
      .set_description("RBSSearch solver tests (RBS, ParSolver(SimpSolver) solve)");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
  ASSERT_FALSE(test_failed);
}

TEST_F(LaunchFixture, simppar_parrbs_test) {
  infra::testing::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("simppar_parrbs.json")
      .set_threads_required(parallel)
      .set_description("ParRBSSearch solver tests (ParRBS, ParSolver(SimpSolver) solve)");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
  ASSERT_FALSE(test_failed);
}

TEST_F(LaunchFixture, lingpar_parrbs_test) {
  infra::testing::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("lingpar_parrbs.json")
      .set_threads_required(parallel)
      .set_description("RBSSearch solver tests (RBS, ParSolver(LingelingSolver) solve)");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
  ASSERT_FALSE(test_failed);
}

TEST_F(LaunchFixture, horde_rbs_test) {
  infra::testing::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("horde_rbs.json")
      .set_threads_required(parallel)
      .set_description("RBSSearch solver tests (ParRBS, HordeSat solve)");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
  ASSERT_FALSE(test_failed);
}

TEST_F(LaunchFixture, horde_parrbs_test) {
  infra::testing::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("horde_parrbs.json")
      .set_threads_required(parallel)
      .set_description("ParRBSSearch solver tests (ParRBS, HordeSat solve)");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
  ASSERT_FALSE(test_failed);
}
