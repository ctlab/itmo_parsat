#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

#include "core/sat/solver/SimpSolver.h"
#include "core/util/CliConfig.h"
#include "core/util/Generator.h"
#include "core/util/stream.h"
#include "core/util/Tracer.h"

TEST(unit_propagation, performance) {
  std::filesystem::path config_path = "./resources/config/naive.json";
  std::filesystem::path cnf_path = "./resources/cnf/unsat_bubble_vs_pancake_7_6_simp-@2.cnf";

  std::ifstream ifs(config_path);
  SolveConfig solve_config;
  core::CliConfig::read_config(ifs, solve_config);

  core::Generator gen(239);
  core::sat::SimpSolver simp_solver(
      solve_config.naive_solve_config().solver_config().simp_solver_config());
  core::sat::Solver& sat_solver = simp_solver;
  simp_solver.parse_cnf(cnf_path);

  int num_tests = 100000;
  while (num_tests--) {
    std::set<int> vars;
    int size = core::random::sample<int>(0, 20);
    Minisat::vec<Minisat::Lit> ms_assumption(size);
    for (int i = 0; i < size; ++i) {
      int var = core::random::sample<int>(0, simp_solver.num_vars() - 1);
      while (vars.find(var) != vars.end()) {
        var = core::random::sample<int>(0, simp_solver.num_vars() - 1);
      }
      int sign = core::random::sample<int>(0, 1);
      ms_assumption[i] = Minisat::mkLit(var, sign);
    }

    Minisat::vec<Minisat::Lit> dummy;
    bool a = IPS_TRACE_V(sat_solver.propagate(ms_assumption, dummy));
    bool b = IPS_TRACE_V(sat_solver.propagate(ms_assumption));
    ASSERT_EQ(a, b);
  }

  core::Tracer::print_summary(10);
}
