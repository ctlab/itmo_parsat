#include <gtest/gtest.h>
#include <filesystem>
#include <functional>

#include "core/sat/SimpBase.h"
#include "core/sat/prop/SimpProp.h"
#include "util/Random.h"
#include "core/tests/common/paths.h"

namespace {

uint64_t num_conflicts_in_subtree(
    Minisat::SimpSolver& solver, Mini::vec<Mini::Lit>& vars, uint32_t index) {
  if (index == (uint32_t) vars.size()) {
    return !solver.prop_check(vars, 0);
  }
  vars[index] = Mini::mkLit(var(vars[index]), false);
  uint64_t sub_false = num_conflicts_in_subtree(solver, vars, index + 1);
  vars[index] = Mini::mkLit(var(vars[index]), true);
  uint64_t sub_true = num_conflicts_in_subtree(solver, vars, index + 1);
  return sub_false + sub_true;
}

core::sat::Problem problem(
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
  common::load_problem(simp_solver, prob);
  auto prop_engine = common::get_prop(common::configs_path + prop_config);
  prop_engine->load_problem(prob);
  int num_tests = 100000;
  while (num_tests--) {
    std::set<int> vars;
    int size = util::random::sample<int>(0, 20);
    Mini::vec<Mini::Lit> ms_assumption(size);
    for (int i = 0; i < size; ++i) {
      int var = util::random::sample<int>(0, simp_solver.nVars() - 1);
      while (vars.find(var) != vars.end()) {
        var = util::random::sample<int>(0, simp_solver.nVars() - 1);
      }
      int sign = util::random::sample<int>(0, 1);
      ms_assumption[i] = Mini::mkLit(var, sign);
    }
    Mini::vec<Mini::Lit> dummy;
    bool expected = !simp_solver.prop_check(ms_assumption, dummy, 0);
    bool actual = prop(*prop_engine, ms_assumption);
    ASSERT_EQ(expected, actual);
  }
}

void test_propagation_tree(
    core::sat::Problem const& prob, std::string const& prop_config,
    std::function<
        uint64_t(core::sat::prop::Prop&, Mini::vec<Mini::Lit> const&, uint32_t)>
        prop) {
  util::random::Generator gen(239);
  Minisat::SimpSolver simp_solver;
  common::load_problem(simp_solver, prob);
  auto prop_engine = common::get_prop(common::configs_path + prop_config);
  prop_engine->load_problem(prob);
  int num_tests = 1000;
  while (num_tests--) {
    std::set<int> vars;
    int size = util::random::sample<int>(0, 20);
    int head_size = util::random::sample<int>(0, size);
    Mini::vec<Mini::Lit> ms_assumption(size);
    for (int i = 0; i < size; ++i) {
      int var = util::random::sample<int>(0, simp_solver.nVars() - 1);
      while (vars.find(var) != vars.end()) {
        var = util::random::sample<int>(0, simp_solver.nVars() - 1);
      }
      int sign = util::random::sample<int>(0, 1);
      ms_assumption[i] = Mini::mkLit(var, sign);
    }
    uint64_t expected =
        num_conflicts_in_subtree(simp_solver, ms_assumption, head_size);
    uint64_t actual = prop(*prop_engine, ms_assumption, head_size);
    ASSERT_EQ(expected, actual);
  }
}

TEST(propagation, correctness_simp_no_dummy) {
  test_propagation_bool(
      problem, "simp_prop.json", [](auto& solver, auto const& assumption) {
        return solver.propagate(assumption);
      });
}

TEST(propagation, correctness_par_no_dummy) {
  test_propagation_bool(
      problem, "par_prop.json", [](auto& solver, auto const& assumption) {
        return solver.propagate(assumption);
      });
}

TEST(propagation, correctness_simp_subtree) {
  test_propagation_tree(
      problem, "simp_prop.json",
      [](auto& solver, auto const& assumption, uint32_t head_size) {
        return solver.prop_tree(assumption, head_size);
      });
}

TEST(propagation, correctness_par_subtree) {
  test_propagation_tree(
      problem, "par_prop.json",
      [](auto& solver, auto const& assumption, uint32_t head_size) {
        return solver.prop_tree(assumption, head_size);
      });
}
