#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

#include "minisat/core/Dimacs.h"
#include "minisat/simp/SimpSolver.h"
#include "core/sat/solver/SimpSolver.h"
#include "core/util/CliConfig.h"
#include "core/util/Generator.h"
#include "core/util/stream.h"
#include "core/util/Tracer.h"
#include "core/util/GzFile.h"

namespace {

uint64_t num_conflicts_in_subtree(
    Minisat::SimpSolver& solver, Minisat::vec<Minisat::Lit>& vars, uint32_t index) {
  if (index == (uint32_t) vars.size()) {
    return !solver.prop_check(vars, 0);
  }
  vars[index] = Minisat::mkLit(var(vars[index]), false);
  uint64_t sub_false = num_conflicts_in_subtree(solver, vars, index + 1);
  vars[index] = Minisat::mkLit(var(vars[index]), true);
  uint64_t sub_true = num_conflicts_in_subtree(solver, vars, index + 1);
  return sub_false + sub_true;
}

}  // namespace

TEST(unit_propagation, correctness_performance) {
  core::Generator gen(239);
  std::filesystem::path config_path = "./resources/config/naive.json";
  std::filesystem::path cnf_path = "./resources/cnf/common/unsat_pancake_vs_selection_7_4-@2.cnf";

  std::ifstream ifs(config_path);
  SolveConfig solve_config;
  core::CliConfig::read_config(ifs, solve_config);

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

TEST(subtree_propagation, correctness_performance) {
  core::Generator gen(239);
  std::filesystem::path cnf_path = "./resources/cnf/common/unsat_pancake_vs_selection_7_4-@2.cnf";
  Minisat::SimpSolver solver;

  {
    core::util::GzFile gz_file(cnf_path);
    solver.parsing = true;
    Minisat::parse_DIMACS(gz_file.native_handle(), solver, true);
    solver.parsing = false;
    solver.eliminate(true);
  }

  int num_tests = 1000;
  while (num_tests--) {
    std::set<int> vars;
    int size = core::random::sample<int>(0, 20);
    int head_size = core::random::sample<int>(0, size);
    Minisat::vec<Minisat::Lit> ms_assumption(size);
    for (int i = 0; i < size; ++i) {
      int var = core::random::sample<int>(0, solver.nVars() - 1);
      while (vars.find(var) != vars.end()) {
        var = core::random::sample<int>(0, solver.nVars() - 1);
      }
      int sign = core::random::sample<int>(0, 1);
      ms_assumption[i] = Minisat::mkLit(var, sign);
    }

    uint64_t fast = IPS_TRACE_V(solver.prop_check_subtree(ms_assumption, head_size));
    uint64_t naive = IPS_TRACE_V(num_conflicts_in_subtree(solver, ms_assumption, head_size));
    ASSERT_EQ(fast, naive);
  }

  core::Tracer::print_summary(10);
}
