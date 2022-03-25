#include "infra/testing/LaunchFixture.h"

#include <filesystem>

TEST_F(LaunchFixture, simppar_recurring_test) {
  infra::domain::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("simppar_recurring.json")
      .set_description(
          "RecurringRBSSearch solver tests (ParSolver(SimpSolver) solve)");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
  ASSERT_FALSE(test_failed);
}

TEST_F(LaunchFixture, painless_recurring_test) {
  infra::domain::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("painless_recurring.json")
      .set_description("RecurringRBSSearch solver tests (Painless solver)");
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
  }
  ASSERT_FALSE(test_failed);
}
