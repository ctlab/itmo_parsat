#include <gtest/gtest.h>
#include <filesystem>
#include <functional>

#include "core/tests/common/util.h"
#include "core/tests/common/paths.h"
#include "core/tests/common/get.h"
#include "core/tests/common/generate.h"

#include "core/sat/prop/SimpProp.h"
#include "util/Random.h"

namespace {

uint64_t num_conflicts_in_subtree(Minisat::SimpSolver& solver, core::lit_vec_t& vars, uint32_t index) {
  if (index == (uint32_t) vars.size()) { return !solver.prop_check(vars, 0); }
  vars[(int) index] = Mini::mkLit(var(vars[(int) index]), false);
  uint64_t sub_false = num_conflicts_in_subtree(solver, vars, index + 1);
  vars[(int) index] = Mini::mkLit(var(vars[(int) index]), true);
  uint64_t sub_true = num_conflicts_in_subtree(solver, vars, index + 1);
  return sub_false + sub_true;
}

}  // namespace

void test_propagation(
    core::sat::Problem const& problem, std::string const& prop_config,
    std::function<void(Minisat::SimpSolver&, core::sat::prop::Prop& prop)> const& f) {
  util::random::Generator gen(239);
  Minisat::SimpSolver base_simp_solver;
  auto prop_engine = common::get_prop(common::configs_path + prop_config);
  common::load_problem(base_simp_solver, problem);
  prop_engine->load_problem(problem);
  f(base_simp_solver, *prop_engine);
}

void test_propagation_bool(
    core::sat::Problem const& problem, std::string const& prop_config,
    std::function<bool(core::sat::prop::Prop& prop, Mini::vec<Mini::Lit> const&)> const& prop) {
  test_propagation(problem, prop_config, [&](auto& base_simp_solver, auto& prop_engine) {
    common::iter_assumptions(
        [&](auto const& assumption) {
          Mini::vec<Mini::Lit> dummy;
          bool expected = !base_simp_solver.prop_check(assumption, dummy, 0);
          bool actual = prop(prop_engine, assumption);
          ASSERT_EQ(expected, actual);
        },
        0, 20, base_simp_solver.nVars() - 1, 1000);
  });
}

void test_propagation_tree(
    core::sat::Problem const& problem, std::string const& prop_config,
    std::function<uint64_t(core::sat::prop::Prop&, Mini::vec<Mini::Lit> const&, uint32_t)> const& prop) {
  test_propagation(problem, prop_config, [&](auto& base_simp_solver, auto& prop_engine) {
    common::iter_assumptions(
        [&](auto& assumption) {
          int head_size = util::random::sample<int>(0, assumption.size());
          uint64_t expected = num_conflicts_in_subtree(base_simp_solver, assumption, head_size);
          uint64_t actual = prop(prop_engine, assumption, head_size);
          ASSERT_EQ(expected, actual);
        },
        0, 20, base_simp_solver.nVars() - 1, 100);
  });
}

DEFINE_PARAMETRIZED_TEST(TestPropagation, std::string, core::sat::Problem, bool /* eliminate */);

static std::vector<std::string> prop_configs{
    "simp_prop.json",
    "par_prop.json",
};

TEST_P(TestPropagation, no_dummy_test) {
  auto [prop_config, problem, eliminate] = GetParam();
  test_propagation_bool(
      problem, prop_config, [](auto& solver, auto const& assumption) { return solver.propagate(assumption); });
}

TEST_P(TestPropagation, tree_test) {
  auto [prop_config, problem, eliminate] = GetParam();
  test_propagation_tree(problem, prop_config, [](auto& solver, auto const& assumption, uint32_t head_size) {
    return solver.prop_tree(assumption, head_size);
  });
}

INSTANTIATE_TEST_CASE_P(
    TestPropagationSmall, TestPropagation,
    ::testing::ValuesIn(common::extend(common::cross(
        common::to_tuple(prop_configs), common::to_tuple(common::problems(false, false, "small")),
        common::to_tuple<bool>({false, true})))));

INSTANTIATE_TEST_CASE_P(
    TestPropagationLarge, TestPropagation,
    ::testing::ValuesIn(common::extend(common::cross(
        common::to_tuple(prop_configs), common::to_tuple(common::problems(false, false, "large")),
        common::to_tuple<bool>({false})))));
