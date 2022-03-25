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
  if (problem.get_result() != core::sat::UNKNOWN) {
    return;
  }
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
      0, 20, base_simp_solver.nVars() - 1, 1000);
}

void test_solver(std::string const& solver_config) {
  util::random::Generator gen(239);
  common::iter_inputs(
      [&](auto const& problem) { test_solver(solver_config, problem); },
      common::BOTH);
}

TEST(SequentialSolver, simp_solver) {
  test_solver("simp_solver.json");
}

TEST(SequentialSolver, maplecomsps_solver) {
  test_solver("maple_solver.json");
}

TEST(SequentialSolver, painless_solver) {
  test_solver("painless_solver.json");
}
