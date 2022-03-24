#include <gtest/gtest.h>
#include <filesystem>

#include "core/tests/common/paths.h"
#include "core/tests/common/generate.h"
#include "core/tests/common/get.h"

#include "core/sat/solver/sequential/Solver.h"
#include "core/sat/native/mini/minisat/minisat/simp/SimpSolver.h"

namespace {

core::sat::State to_state(Minisat::lbool b) {
  if (b == Minisat::l_True) {
    return core::sat::SAT;
  } else if (b == Minisat::l_False) {
    return core::sat::UNSAT;
  } else {
    return core::sat::UNKNOWN;
  }
}

}  // namespace

void test_solver(
    std::string const& solver_config, core::sat::Problem const& problem) {
  IPS_INFO("Formula: " << problem.path());
  core::sat::solver::RSolver solver =
      common::get_solver(common::configs_path + solver_config);
  Minisat::SimpSolver base_simp_solver;
  solver->load_problem(problem);
  common::load_problem(base_simp_solver, problem);

  common::pass_assumptions(
      [&](auto const& assumption) {
        auto expected =
            to_state(base_simp_solver.solveLimited(assumption, false, true));
        auto actual = solver->solve(assumption);
        ASSERT_EQ(expected, actual);
      },
      0, 20, base_simp_solver.nVars() - 1, 100);
}

void test_solver(std::string const& solver_config, bool eliminate) {
  util::random::Generator gen(239);
  for (auto input : common::inputs) {
    IPS_INFO("Testing " << solver_config);
    FS_PATH_INPUT(problem_path, input);
    core::sat::Problem problem(problem_path, eliminate);
    if (problem.get_result() == core::sat::UNKNOWN) {
      test_solver(solver_config, problem);
    }
  }
}

TEST(SequentialSolver, simp_solver_no_elim) {
  test_solver("simp_solver.json", false);
}

TEST(SequentialSolver, painless_solver_no_elim) {
  test_solver("painless_solver.json", false);
}

TEST(SequentialSolver, maplecomsps_solver_no_elim) {
  test_solver("maple_solver.json", false);
}

TEST(SequentialSolver, simp_solver_elim) {
  test_solver("simp_solver.json", true);
}

TEST(SequentialSolver, painless_solver_elim) {
  test_solver("painless_solver.json", true);
}

TEST(SequentialSolver, maplecomsps_solver_elim) {
  test_solver("maple_solver.json", true);
}
