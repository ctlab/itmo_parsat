#include <gtest/gtest.h>
#include <filesystem>

#include "core/tests/common/paths.h"
#include "core/tests/common/generate.h"
#include "core/tests/common/get.h"
#include "core/tests/common/util.h"

#include "core/sat/solver/service/SolverService.h"

void working_thread(
    core::sat::solver::SolverService& service,
    core::sat::Problem const& problem, int num_tests, bool time_limit) {
  using namespace std::chrono;
  using namespace std::chrono_literals;
  util::random::Generator gen(1337);
  Minisat::SimpSolver base_simp_solver;
  common::load_problem(base_simp_solver, problem);
  common::iter_assumptions(
      [&](auto const& assumption) {
        duration tl = time_limit
                          ? milliseconds(util::random::sample<int>(100, 500))
                          : core::sat::solver::SolverService::DUR_INDEF;
        core::clock_t::time_point start = core::clock_t::now();
        auto actual = service.solve(assumption, tl).get();
        core::clock_t::duration dur = core::clock_t::now() - start;
        if (time_limit) {
          ASSERT_LE(dur, tl + 1000ms);
        }
        auto expected = core::sat::UNKNOWN;
        if (!time_limit || actual != core::sat::UNKNOWN) {
          expected = common::to_state(
              base_simp_solver.solveLimited(assumption, false, true));
        }
        ASSERT_EQ(expected, actual);
      },
      0, 20, base_simp_solver.nVars() - 1, num_tests);
}

void test_solver_service(
    std::string const& service_config, std::string const& input_name,
    bool eliminate, size_t n_workers, int num_tests, bool time_limit) {
  util::random::Generator gen(239);
  auto problem = common::get_problem(input_name, eliminate);
  if (problem.get_result() != core::sat::UNKNOWN) {
    return;
  }
  core::sat::solver::RSolverService service =
      common::get_solver_service(common::configs_path + service_config);
  service->load_problem(problem);
  std::vector<std::thread> working_threads;
  for (size_t i = 0; i < n_workers; ++i) {
    working_threads.emplace_back([&service, &problem, num_tests, time_limit] {
      working_thread(*service, problem, num_tests, time_limit);
    });
  }
  for (auto& thread : working_threads) {
    thread.join();
  }
}

DEFINE_PARAMETRIZED_TEST(
    TestSolverService, std::string, std::string, bool /* elim */,
    int /* workers */, int /* num_tests */, bool /* time_limit */);

static std::vector<std::string> service_configs{
    "simp_service_1.json",     "simp_service_16.json",
    "maple_service_1.json",    "maple_service_16.json",
    "painless_service_1.json", "painless_service_4.json",
};

TEST_P(TestSolverService, correctness) {
  auto [service_config, input_name, eliminate, workers, num_tests, time_limit] =
      GetParam();
  test_solver_service(
      service_config, input_name, eliminate, workers, num_tests, time_limit);
}

INSTANTIATE_TEST_CASE_P(
    SolverServiceSmallNoTimeLimit, TestSolverService,
    ::testing::ValuesIn(common::extend(
        common::cross(
            common::to_tuple(service_configs),
            common::to_tuple(common::small_inputs),
            common::to_tuple<bool>({false, true})),  // eliminate
        common::to_tuple<int>({16}),                 // num workers
        common::to_tuple<int>({1000}),               // num tests
        common::to_tuple<bool>({false}))));          // time_limit

INSTANTIATE_TEST_CASE_P(
    SolverServiceLargeWithTimeLimit, TestSolverService,
    ::testing::ValuesIn(common::extend(
        common::cross(
            common::to_tuple(service_configs),
            common::to_tuple(common::large_inputs),
            common::to_tuple<bool>({false})),  // no eliminate (takes long)
        common::to_tuple<int>({1}),            // num workers
        common::to_tuple<int>({5}),            // num tests
        common::to_tuple<bool>({true}))));     // time_limit
