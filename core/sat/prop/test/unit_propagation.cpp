#include <gtest/gtest.h>
#include <filesystem>
#include <functional>

#include "core/tests/common/paths.h"
#include "core/tests/common/get.h"
#include "core/tests/common/generate.h"

#include "core/sat/SimpBase.h"
#include "core/sat/prop/SimpProp.h"
#include "util/Random.h"

namespace {

uint64_t num_conflicts_in_subtree(
    Minisat::SimpSolver& solver, core::lit_vec_t& vars, uint32_t index) {
  if (index == (uint32_t) vars.size()) {
    return !solver.prop_check(vars, 0);
  }
  vars[(int) index] = Mini::mkLit(var(vars[(int) index]), false);
  uint64_t sub_false = num_conflicts_in_subtree(solver, vars, index + 1);
  vars[(int) index] = Mini::mkLit(var(vars[(int) index]), true);
  uint64_t sub_true = num_conflicts_in_subtree(solver, vars, index + 1);
  return sub_false + sub_true;
}

std::filesystem::path problem(
    IPS_PROJECT_ROOT
    "/resources/cnf/common/unsat_pancake_vs_selection_7_4-@2.cnf");

}  // namespace

void test_propagation_bool(
    core::sat::Problem const& prob, std::string const& prop_config,
    std::function<
        bool(core::sat::prop::Prop& prop, Mini::vec<Mini::Lit> const&)> const&
        prop) {
  util::random::Generator gen(239);
  Minisat::SimpSolver simp_solver;
  auto prop_engine = common::get_prop(common::configs_path + prop_config);
  common::load_problem(simp_solver, prob);
  prop_engine->load_problem(prob);

  common::pass_assumptions(
      [&](auto const& assumption) {
        Mini::vec<Mini::Lit> dummy;
        bool expected = !simp_solver.prop_check(assumption, dummy, 0);
        bool actual = prop(*prop_engine, assumption);
        ASSERT_EQ(expected, actual);
      },
      0, 20, simp_solver.nVars() - 1, 100000);
}

void test_propagation_tree(
    core::sat::Problem const& prob, std::string const& prop_config,
    std::function<uint64_t(
        core::sat::prop::Prop&, Mini::vec<Mini::Lit> const&, uint32_t)> const&
        prop) {
  util::random::Generator gen(239);
  Minisat::SimpSolver simp_solver;
  auto prop_engine = common::get_prop(common::configs_path + prop_config);
  common::load_problem(simp_solver, prob);
  prop_engine->load_problem(prob);

  common::pass_assumptions(
      [&](auto& assumption) {
        int head_size = util::random::sample<int>(0, assumption.size());
        uint64_t expected =
            num_conflicts_in_subtree(simp_solver, assumption, head_size);
        uint64_t actual = prop(*prop_engine, assumption, head_size);
        ASSERT_EQ(expected, actual);
      },
      0, 20, simp_solver.nVars() - 1, 1000);
}

TEST(propagation, correctness_simp_no_dummy) {
  test_propagation_bool(
      core::sat::Problem(problem), "simp_prop.json",
      [](auto& solver, auto const& assumption) {
        return solver.propagate(assumption);
      });
}

TEST(propagation, correctness_par_no_dummy) {
  test_propagation_bool(
      core::sat::Problem(problem), "par_prop.json",
      [](auto& solver, auto const& assumption) {
        return solver.propagate(assumption);
      });
}

TEST(propagation, correctness_simp_subtree) {
  test_propagation_tree(
      core::sat::Problem(problem), "simp_prop.json",
      [](auto& solver, auto const& assumption, uint32_t head_size) {
        return solver.prop_tree(assumption, head_size);
      });
}

TEST(propagation, correctness_par_subtree) {
  test_propagation_tree(
      core::sat::Problem(problem), "par_prop.json",
      [](auto& solver, auto const& assumption, uint32_t head_size) {
        return solver.prop_tree(assumption, head_size);
      });
}
