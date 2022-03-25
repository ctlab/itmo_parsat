#include <gtest/gtest.h>
#include <filesystem>

#include "core/tests/common/paths.h"
#include "core/tests/common/generate.h"
#include "core/tests/common/get.h"
#include "core/tests/common/util.h"

#include "core/sat/solver/service/SolverService.h"

void working_thread(
    core::sat::solver::SolverService& service,
    core::sat::Problem const& problem) {
  util::random::Generator gen(1337);
  Minisat::SimpSolver base_simp_solver;
  common::load_problem(base_simp_solver, problem);
  common::iter_assumptions(
      [&](auto const& assumption) {
        auto expected = common::to_state(
            base_simp_solver.solveLimited(assumption, false, true));
        auto actual = service.solve(assumption, std::chrono::hours(24)).get();
        ASSERT_EQ(expected, actual);
      },
      0, 20, base_simp_solver.nVars() - 1, 1000);
}

void test_solver_service(
    std::string const& config, core::sat::Problem const& problem,
    size_t n_workers) {
  core::sat::solver::RSolverService service =
      common::get_solver_service(common::configs_path + config);
  service->load_problem(problem);
  std::vector<std::thread> working_threads;
  for (size_t i = 0; i < n_workers; ++i) {
    working_threads.emplace_back(
        [&service, &problem] { working_thread(*service, problem); });
  }
  for (auto& thread : working_threads) {
    thread.join();
  }
}

void test_solver_service(std::string const& config, size_t n_workers) {
  util::random::Generator gen(239);
  for (auto input : common::inputs) {
    FS_PATH_INPUT(problem_path, input);
    core::sat::Problem problem(problem_path, false);
    if (problem.get_result() == core::sat::UNKNOWN) {
      test_solver_service(config, problem, n_workers);
    }
  }
}

TEST(SolverService, seq_simp_1) {
  test_solver_service("simp_service_1.json", 16);
}

TEST(SolverService, seq_simp_16) {
  test_solver_service("simp_service_16.json", 16);
}

TEST(SolverService, seq_maple_1) {
  test_solver_service("maple_service_1.json", 16);
}

TEST(SolverService, seq_maple_16) {
  test_solver_service("maple_service_16.json", 16);
}

TEST(SolverService, seq_painless_1) {
  test_solver_service("painless_service_1.json", 16);
}

TEST(SolverService, seq_painless_4) {
  test_solver_service("painless_service_4.json", 16);
}
