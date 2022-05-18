#include "infra/fixture/LaunchFixture.h"

#include "core/tests/common/util.h"

INSTANTIATE_TEST_SUITE_P(
    TestWithoutPainless16CPU, LaunchFixture,
    ::testing::ValuesIn(common::smap<LaunchFixture::ParamType>(common::cross(
        common::to_tuple<int>({16}), common::to_tuple<std::string>({"wop"}),
        common::to_tuple<std::string>({"s1-16", "m1-16", "ms1-15", "p15-1", "p5-3", "ps5-3"}),
        common::to_tuple<std::string>({"sing", "rec"}), common::to_tuple<std::string>({"rbs", "cart"}),
        common::to_tuple<std::string>({"ea", "ga"})))));

INSTANTIATE_TEST_SUITE_P(
    TestWithPainless16CPU, LaunchFixture,
    ::testing::ValuesIn(common::smap<LaunchFixture::ParamType>(common::cross(
        common::to_tuple<int>({16}), common::to_tuple<std::string>({"wp"}),
        common::to_tuple<std::string>({"s1-11", "m1-11", "ms1-10", "p10-1", "p5-2", "ps5-2"}),
        common::to_tuple<std::string>({"sing", "rec"}), common::to_tuple<std::string>({"rbs", "cart"}),
        common::to_tuple<std::string>({"ea", "ga"})))));

INSTANTIATE_TEST_SUITE_P(
    TestWithoutPainless32CPU, LaunchFixture,
    ::testing::ValuesIn(common::smap<LaunchFixture::ParamType>(common::cross(
        common::to_tuple<int>({32}), common::to_tuple<std::string>({"wop"}),
        common::to_tuple<std::string>({"s1-32", "m1-32", "ms1-31", "p31-1", "p6-5", "ps6-5"}),
        common::to_tuple<std::string>({"sing", "rec"}), common::to_tuple<std::string>({"rbs", "cart"}),
        common::to_tuple<std::string>({"ea", "ga"})))));

INSTANTIATE_TEST_SUITE_P(
    TestWithPainless32CPU, LaunchFixture,
    ::testing::ValuesIn(common::smap<LaunchFixture::ParamType>(common::cross(
        common::to_tuple<int>({32}), common::to_tuple<std::string>({"wp"}),
        common::to_tuple<std::string>({"s1-21", "m1-21", "ms1-20", "p20-1", "p5-4", "ps5-4"}),
        common::to_tuple<std::string>({"sing", "rec"}), common::to_tuple<std::string>({"rbs", "cart"}),
        common::to_tuple<std::string>({"ea", "ga"})))));
