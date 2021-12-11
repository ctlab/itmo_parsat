#include "infra/include/testing/LaunchFixture.h"

TEST_F(LaunchFixture, base_test_no_backdoor) {
  infra::testing::LaunchConfig config;
  config.expected_result(infra::domain::UNSAT).config_path("base.json");
  launch(config.input_path("pancake_vs_selection_7_4.cnf"));
  launch(config.input_path("bubble_vs_pancake_7_6_simp.cnf"));
  launch(config.input_path("sgen6_900_100.cnf"));
}

TEST_F(LaunchFixture, base_test_backdoor) {
  infra::testing::LaunchConfig config;
  config.backdoor(true).config_path("base.json").expected_result(infra::domain::UNSAT);
  launch(config.input_path("pancake_vs_selection_7_4.cnf"));
  launch(config.input_path("bubble_vs_pancake_7_6_simp.cnf"));
  launch(config.input_path("sgen6_900_100.cnf"));
}
