#include <gtest/gtest.h>
#include <filesystem>

#include "core/tests/common/util.h"
#include "core/tests/common/paths.h"
#include "core/tests/common/get.h"
#include "core/tests/common/generate.h"

#include "core/evol/algorithm/Algorithm.h"
#include "util/Random.h"
#include "util/mini.h"
#include "util/stream.h"

void check_rho_value(
    core::sat::prop::RProp const& rprop,
    ea::preprocess::RPreprocess const& preprocess,
    ea::instance::Instance const& instance,
    core::lit_vec_t const& base_assumption) {
  uint32_t samples = instance.fitness().samples;
  uint32_t size = instance.size();
  if (!instance.fitness().can_calc() || (1ULL << size) != samples) {
    return;
  }
  auto search = core::search::createFullSearch(
      preprocess->var_view(), instance.get_vars().get_mask());
  uint32_t conflicts = rprop->prop_tree(
      util::concat(base_assumption, (*search)()), base_assumption.size());
  double expected_rho = instance.fitness().rho;
  double actual_rho = (double) conflicts / (double) (1ULL << instance.size());
  ASSERT_NEAR(expected_rho, actual_rho, 1e-8);
}

void run_test(
    ea::preprocess::RPreprocess const& preprocess, std::string const& config,
    core::sat::Problem const& problem, core::lit_vec_t const& base_assumption) {
  auto rprop = common::get_prop(common::configs_path + "par_prop.json");
  auto algorithm = common::get_algorithm(rprop, common::configs_path + config);
  rprop->load_problem(problem);
  algorithm->prepare(preprocess);
  algorithm->set_base_assumption(base_assumption);
  algorithm->process();
  check_rho_value(rprop, preprocess, algorithm->get_best(), base_assumption);
}

void run_test(std::string const& config, core::sat::Problem const& problem) {
  common::set_logger_config();
  util::random::Generator gen(239);
  auto preprocess = common::get_preprocess();
  if (!preprocess->preprocess(problem)) {
    return;
  }
  run_test(preprocess, config, problem, {});
  common::iter_assumptions(
      [&](auto& base_assumption) {
        run_test(preprocess, config, problem, base_assumption);
      },
      preprocess->var_view(), 0, 20, 100);
}

DEFINE_PARAMETRIZED_TEST(
    TestAlgorithm, std::string /* config */, core::sat::Problem /* problem */);

static std::vector<std::string> algorithm_configs{
    "ea_unit.json",
    "ga_unit.json",
};

TEST_P(TestAlgorithm, correctness) {
  auto [algorithm_config, problem] = GetParam();
  run_test(algorithm_config, problem);
}

INSTANTIATE_TEST_CASE_P(
    TestAlgorithm, TestAlgorithm,
    ::testing::ValuesIn(common::extend(common::cross(
        common::to_tuple(algorithm_configs),
        common::to_tuple(common::problems(false, false, "small"))))));
