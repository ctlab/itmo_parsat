#include "infra/fixture/LaunchFixture.h"

#include <sstream>
#include <filesystem>

#include "core/tests/common/util.h"

namespace {

std::string get_config_name(
    std::tuple<int, std::string, std::string, std::string, std::string> const&
        tup) {
  auto [cpu, reduce, rbs, alg, slv] = tup;
  std::stringstream ss;
  ss << cpu << "_" << reduce << "_" << rbs << "_" << alg << "_" << slv
     << ".json";
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
    TestAll16CPU, LaunchFixture,
    ::testing::ValuesIn(common::cross(
        common::to_tuple<int>({16}),
        common::to_tuple<std::string>({"rec", "sing"}),
        common::to_tuple<std::string>({"cart", "rbs"}),
        common::to_tuple<std::string>({"ea", "ga"}),
        common::to_tuple<std::string>({"s1-16", "p16-1", "p4-4"}))));
