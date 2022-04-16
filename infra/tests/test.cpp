#include "infra/fixture/LaunchFixture.h"

#include <sstream>
#include <filesystem>

#include "core/tests/common/util.h"

namespace {

std::string get_config_name(std::tuple<
                            int, std::string, std::string, std::string,
                            std::string, std::string> const& tup) {
  auto [cpu, pain, slv, reduce, rbs, alg] = tup;
  std::stringstream ss;
  ss << cpu << "_" << pain << "_" << slv << "_" << reduce << "_" << rbs << "_"
     << alg << ".json";
  return ss.str();
}

}  // namespace

TEST_P(LaunchFixture, infra_test) {
  infra::domain::LaunchConfig config;
  config.set_expected_result(infra::domain::UNKNOWN)
      .set_log_config_path("log.json")
      .set_config_path(get_config_name(GetParam()));
  for (const auto& path : LaunchFixture::cnfs) {
    launch(config.set_input_path(path));
    if (is_interrupted) {
      break;
    }
    ASSERT_FALSE(test_failed);
  }
}

INSTANTIATE_TEST_CASE_P(
    TestWithoutPainless16CPU, LaunchFixture,
    ::testing::ValuesIn(common::cross(
        common::to_tuple<int>({16}), common::to_tuple<std::string>({"wop"}),
        common::to_tuple<std::string>(
            {"s1-16", "m1-16", "ms1-15", "p15-1", "p5-3", "ps5-3"}),
        common::to_tuple<std::string>({"sing", "rec"}),
        common::to_tuple<std::string>({"rbs", "cart"}),
        common::to_tuple<std::string>({"ea", "ga"}))));

INSTANTIATE_TEST_CASE_P(
    TestWithPainless16CPU, LaunchFixture,
    ::testing::ValuesIn(common::cross(
        common::to_tuple<int>({16}), common::to_tuple<std::string>({"wp"}),
        common::to_tuple<std::string>(
            {"s1-11", "m1-11", "ms1-10", "p10-1", "p5-2", "ps5-2"}),
        common::to_tuple<std::string>({"sing", "rec"}),
        common::to_tuple<std::string>({"rbs", "cart"}),
        common::to_tuple<std::string>({"ea", "ga"}))));

INSTANTIATE_TEST_CASE_P(
    TestWithoutPainless32CPU, LaunchFixture,
    ::testing::ValuesIn(common::cross(
        common::to_tuple<int>({32}), common::to_tuple<std::string>({"wop"}),
        common::to_tuple<std::string>(
            {"s1-32", "m1-32", "ms1-31", "p31-1", "p6-5", "ps6-5"}),
        common::to_tuple<std::string>({"sing", "rec"}),
        common::to_tuple<std::string>({"rbs", "cart"}),
        common::to_tuple<std::string>({"ea", "ga"}))));

INSTANTIATE_TEST_CASE_P(
    TestWithPainless32CPU, LaunchFixture,
    ::testing::ValuesIn(common::cross(
        common::to_tuple<int>({32}), common::to_tuple<std::string>({"wp"}),
        common::to_tuple<std::string>(
            {"s1-21", "m1-21", "ms1-20", "p20-1", "p5-4", "ps5-4"}),
        common::to_tuple<std::string>({"sing", "rec"}),
        common::to_tuple<std::string>({"rbs", "cart"}),
        common::to_tuple<std::string>({"ea", "ga"}))));

/* Experiments definition */

INSTANTIATE_TEST_CASE_P(
    Experiment_1_32CPU_EA, LaunchFixture,
    ::testing::ValuesIn(common::cross(
        common::to_tuple<int>({32}), common::to_tuple<std::string>({"wp"}),
        common::to_tuple<std::string>({"s1-21", "ms1-20", "ps5-4"}),
        common::to_tuple<std::string>({"sing", "rec"}),
        common::to_tuple<std::string>({"rbs", "cart"}),
        common::to_tuple<std::string>({"ea"}))));

INSTANTIATE_TEST_CASE_P(
    Experiment_1_32CPU_GA, LaunchFixture,
    ::testing::ValuesIn(common::cross(
        common::to_tuple<int>({32}), common::to_tuple<std::string>({"wp"}),
        common::to_tuple<std::string>({"s1-21", "ms1-20", "ps5-4"}),
        common::to_tuple<std::string>({"sing", "rec"}),
        common::to_tuple<std::string>({"rbs", "cart"}),
        common::to_tuple<std::string>({"ga"}))));

INSTANTIATE_TEST_CASE_P(
    Experiment_AAAI_reproduce_wop, LaunchFixture,
    ::testing::ValuesIn(common::cross(
        common::to_tuple<int>({32}), common::to_tuple<std::string>({"wop"}),
        common::to_tuple<std::string>({"s1-32", "ms1-31", "ps6-5"}),
        common::to_tuple<std::string>({"sing", "rec"}),
        common::to_tuple<std::string>({"rbs", "cart"}),
        common::to_tuple<std::string>({"ea", "ga"}))));

INSTANTIATE_TEST_CASE_P(
    Experiment_AAAI_reproduce_wp, LaunchFixture,
    ::testing::ValuesIn(common::cross(
        common::to_tuple<int>({32}), common::to_tuple<std::string>({"wp"}),
        common::to_tuple<std::string>({"s1-21", "ms1-20", "ps5-4"}),
        common::to_tuple<std::string>({"sing", "rec"}),
        common::to_tuple<std::string>({"rbs", "cart"}),
        common::to_tuple<std::string>({"ea", "ga"}))));
