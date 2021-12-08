#include "infra/include/testing/LaunchFixture.h"

TEST_F(LaunchFixture, check_test) {
  // clang-format off
  LaunchConfig config;
  launch(config
    .backdoor(true)
    .input_path("pancake_vs_selection_7_4.cnf")
    .config_path("base.json")
  );
  // clang-format on
}
