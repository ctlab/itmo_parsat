#include "infra/fixture/LaunchFixture.h"

#include "core/tests/common/util.h"

INSTANTIATE_TEST_SUITE_P(
    Experiment_1_32CPU_EA, LaunchFixture,
    ::testing::ValuesIn(common::smap<LaunchFixture::ParamType>(common::cross(
        common::to_tuple<int>({32}), common::to_tuple<std::string>({"wp"}),
        common::to_tuple<std::string>({"s1-21", "ms1-20", "ps5-4"}), common::to_tuple<std::string>({"sing", "rec"}),
        common::to_tuple<std::string>({"rbs", "cart"}), common::to_tuple<std::string>({"ea"})))));

INSTANTIATE_TEST_SUITE_P(
    Experiment_1_32CPU_GA, LaunchFixture,
    ::testing::ValuesIn(common::smap<LaunchFixture::ParamType>(common::cross(
        common::to_tuple<int>({32}), common::to_tuple<std::string>({"wp"}),
        common::to_tuple<std::string>({"s1-21", "ms1-20", "ps5-4"}), common::to_tuple<std::string>({"sing", "rec"}),
        common::to_tuple<std::string>({"rbs", "cart"}), common::to_tuple<std::string>({"ga"})))));

INSTANTIATE_TEST_SUITE_P(
    Experiment_AAAI_reproduce_wop_EA, LaunchFixture,
    ::testing::ValuesIn(common::smap<LaunchFixture::ParamType>(common::cross(
        common::to_tuple<int>({32}), common::to_tuple<std::string>({"wop"}),
        common::to_tuple<std::string>({"s1-32", "ms1-31", "ps6-5"}), common::to_tuple<std::string>({"sing", "rec"}),
        common::to_tuple<std::string>({"rbs", "cart"}), common::to_tuple<std::string>({"ea"})))));

INSTANTIATE_TEST_SUITE_P(
    Experiment_AAAI_reproduce_wp_EA, LaunchFixture,
    ::testing::ValuesIn(common::smap<LaunchFixture::ParamType>(common::cross(
        common::to_tuple<int>({32}), common::to_tuple<std::string>({"wp"}),
        common::to_tuple<std::string>({"s1-21", "ms1-20", "ps5-4"}), common::to_tuple<std::string>({"sing", "rec"}),
        common::to_tuple<std::string>({"rbs", "cart"}), common::to_tuple<std::string>({"ea"})))));

INSTANTIATE_TEST_SUITE_P(
    Experiment_AAAI_reproduce_wop_GA, LaunchFixture,
    ::testing::ValuesIn(common::smap<LaunchFixture::ParamType>(common::cross(
        common::to_tuple<int>({32}), common::to_tuple<std::string>({"wop"}),
        common::to_tuple<std::string>({"s1-32", "ms1-31", "ps6-5"}), common::to_tuple<std::string>({"sing", "rec"}),
        common::to_tuple<std::string>({"rbs", "cart"}), common::to_tuple<std::string>({"ga"})))));

INSTANTIATE_TEST_SUITE_P(
    Experiment_AAAI_reproduce_wp_GA, LaunchFixture,
    ::testing::ValuesIn(common::smap<LaunchFixture::ParamType>(common::cross(
        common::to_tuple<int>({32}), common::to_tuple<std::string>({"wp"}),
        common::to_tuple<std::string>({"s1-21", "ms1-20", "ps5-4"}), common::to_tuple<std::string>({"sing", "rec"}),
        common::to_tuple<std::string>({"rbs", "cart"}), common::to_tuple<std::string>({"ga"})))));
