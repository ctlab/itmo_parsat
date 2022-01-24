#include "infra/testing/LaunchFixture.h"

#include <filesystem>

namespace {

template <typename T, typename F>
void apply(std::optional<T>&& opt, F&& func) {
  if (opt.has_value()) {
    func(std::move(opt));
  }
}

void await(std::optional<std::shared_ptr<infra::Execution>>&& exec_opt) {
  exec_opt.value()->await();
}

}  // namespace

TEST_F(LaunchFixture, naive_test) {
  // For now, ignore this test because it is too large
  infra::testing::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("naive.json")
      .set_description("Naive solver tests (no RBS)");
  for (const auto& path : LaunchFixture::cnfs) {
    // Executions use only one thread so can be launched concurrently
    launch(config.set_input_path(path));
  }
  ASSERT_FALSE(test_failed);
}

TEST_F(LaunchFixture, simp_rbs_test) {
  infra::testing::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("simp_rbs.json")
      .set_description("RBSSearch solver tests (RBS, sequential solve)");
  for (const auto& path : LaunchFixture::cnfs) {
    // We wait for execution before proceeding because it utilizes high concurrency
    apply(launch(config.set_input_path(path)), await);
  }
  ASSERT_FALSE(test_failed);
}

TEST_F(LaunchFixture, simp_par_rbs_test) {
  infra::testing::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("simp_par_rbs.json")
      .set_description("ParRBSSearch solver tests (ParRBS, sequential solve)");
  for (const auto& path : LaunchFixture::cnfs) {
    // We wait for execution before proceeding because it utilizes high concurrency
    apply(launch(config.set_input_path(path)), await);
  }
  ASSERT_FALSE(test_failed);
}

TEST_F(LaunchFixture, par_rbs_test) {
  infra::testing::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("par_rbs.json")
      .set_description("RBSSearch solver tests (RBS, parallel solve)");
  for (const auto& path : LaunchFixture::cnfs) {
    // We wait for execution before proceeding because it utilizes high concurrency
    apply(launch(config.set_input_path(path)), await);
  }
  ASSERT_FALSE(test_failed);
}

TEST_F(LaunchFixture, par_par_rbs_test) {
  infra::testing::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path("par_par_rbs.json")
      .set_description("ParRBSSearch solver tests (ParRBS, parallel solve)");
  for (const auto& path : LaunchFixture::cnfs) {
    // We wait for execution before proceeding because it utilizes high concurrency
    apply(launch(config.set_input_path(path)), await);
  }
  ASSERT_FALSE(test_failed);
}
