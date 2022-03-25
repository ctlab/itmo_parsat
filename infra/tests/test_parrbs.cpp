#include "infra/testing/LaunchFixture.h"

#include <filesystem>

TEST_F(LaunchFixture, simppar_parrbs_test) {
  infra::domain::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("simppar_parrbs.json")
      .set_description(
          "ParRBSSearch solver tests (ParRBS, ParSolver(SimpSolver) solve)");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
  ASSERT_FALSE(test_failed);
}

TEST_F(LaunchFixture, painless_parrbs_test) {
  infra::domain::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("painless_parrbs.json")
      .set_description("ParRBSSearch solver tests (ParRBS, Painless solver)");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
  ASSERT_FALSE(test_failed);
}
