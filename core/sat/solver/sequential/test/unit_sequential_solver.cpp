#include <gtest/gtest.h>
#include <filesystem>

#include "core/tests/common/paths.h"
#include "core/tests/common/generate.h"
#include "core/tests/common/get.h"
#include "core/tests/common/util.h"

#include "core/sat/solver/sequential/Solver.h"
#include "core/sat/native/mini/minisat/minisat/simp/SimpSolver.h"

void test_solver(
    std::string const& solver_config, core::sat::Problem const& problem) {
  util::random::Generator gen(239);
  core::sat::solver::RSolver solver =
      common::get_solver(common::configs_path + solver_config);
  Minisat::SimpSolver base_simp_solver;
  solver->load_problem(problem);
  common::load_problem(base_simp_solver, problem);
  common::iter_assumptions(
      [&](auto const& assumption) {
        auto expected = common::to_state(
            base_simp_solver.solveLimited(assumption, false, true));
        auto actual = solver->solve(assumption);
        ASSERT_EQ(expected, actual);
      },
      0, 20, base_simp_solver.nVars() - 1, 16000);
}

DEFINE_PARAMETRIZED_TEST(
    TestSequentialSolver, std::string /* config */,
    core::sat::Problem /* problem */);

static std::vector<std::string> solver_configs{
    "simp_solver.json",
    "maple_solver.json",
    "painless_solver.json",
};

TEST_P(TestSequentialSolver, correctness) {
  auto [solver_config, problem] = GetParam();
  test_solver(solver_config, problem);
}

INSTANTIATE_TEST_CASE_P(
    TestSequentialSolver, TestSequentialSolver,
    ::testing::ValuesIn(common::cross(
        common::to_tuple(solver_configs),
        common::to_tuple(common::concat(
            common::problems(false, false, "small"),
            common::problems(true, false, "small"))))));
