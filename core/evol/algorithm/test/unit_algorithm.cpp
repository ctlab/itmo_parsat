#include <gtest/gtest.h>
#include <filesystem>

#include "core/tests/common/paths.h"
#include "core/tests/common/get.h"
#include "core/tests/common/generate.h"

#include "core/evol/algorithm/Algorithm.h"
#include "util/Random.h"
#include "util/mini.h"
#include "util/stream.h"

#define NUM_BASE_ASSUMPTIONS 10

void check_correct(
    core::sat::prop::RProp rprop, ea::preprocess::RPreprocess const& preprocess,
    ea::instance::Instance const& instance,
    Mini::vec<Mini::Lit> const& base_assumps) {
  double rho_value = instance.fitness().rho;
  uint32_t samples = instance.fitness().samples;
  uint32_t size = instance.size();
  if (!instance.fitness().can_calc() || (1ULL << size) != samples) {
    IPS_INFO("Inaccurate rho value, skip.");
    return;
  }
  Mini::vec<Mini::Lit> base_assump;
  std::vector<Mini::vec<Mini::Lit>> assumptions;
  auto p_search = core::domain::createFullSearch(
      preprocess->var_view(), instance.get_vars().get_mask());
  auto& search = *p_search;
  uint32_t conflicts =
      rprop->prop_tree(util::concat(base_assump, search()), base_assump.size());
  ASSERT_NEAR(
      instance.fitness().rho, (double) conflicts / (1ULL << instance.size()),
      1e-8);
}

void run_test(
    ea::preprocess::RPreprocess preprocess,
    std::string const& algorithm_config_name, core::sat::Problem const& problem,
    Mini::vec<Mini::Lit> const& base_assumption) {
  auto rprop = common::get_prop(common::configs_path + "par_prop.json");
  auto algorithm = common::get_algorithm(
      rprop, common::configs_path + algorithm_config_name);
  rprop->load_problem(problem);
  algorithm->prepare(preprocess);
  algorithm->process();
  auto const& best = algorithm->get_best();
  IPS_INFO("Best: " << best);
  check_correct(rprop, preprocess, best, base_assumption);
}

void run_base_assumptions(
    ea::preprocess::RPreprocess preprocess,
    std::string const& algorithm_config_name,
    core::sat::Problem const& problem) {
  run_test(preprocess, algorithm_config_name, problem, {});
  int num_vars = preprocess->var_view().size();
  for (int i = 0; i < NUM_BASE_ASSUMPTIONS; ++i) {
    int size = util::random::sample<int>(1, num_vars - 1);
    Mini::vec<Mini::Lit> assumption(size);
    for (int j = 0; j < size; ++j) {
      assumption[j] = Mini::mkLit(
          preprocess->var_view()[util::random::sample<int>(0, num_vars - 1)],
          util::random::sample<int>(0, 1));
    }
    run_test(preprocess, algorithm_config_name, problem, assumption);
  }
}

void run_test(
    std::string const& algorithm_config_name,
    std::filesystem::path const& input) {
  common::set_logger_config();
  util::random::Generator gen(239);

  core::sat::Problem problem(input);
  auto preprocess = common::get_preprocess();
  if (!preprocess->preprocess(problem)) {
    IPS_INFO("Skipped: " << input);
  }
  run_base_assumptions(preprocess, algorithm_config_name, problem);
}

TEST(algorithm, ea) {
  std::filesystem::path input = IPS_PROJECT_ROOT
      "/resources/cnf/common/unsat_pancake_vs_selection_7_4-@2.cnf";
  run_test("ea.json", input);
}

TEST(algorithm, ga) {
  std::filesystem::path input = IPS_PROJECT_ROOT
      "/resources/cnf/common/unsat_pancake_vs_selection_7_4-@2.cnf";
  run_test("ga.json", input);
}