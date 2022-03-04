#include <gtest/gtest.h>
#include <filesystem>

#include "core/sat/SimpBase.h"
#include "core/sat/prop/SimpProp.h"
#include "core/sat/solver/MapleCOMSPSSolver.h"
#include "util/Random.h"
#include "util/TimeTracer.h"
#include "util/GzFile.h"

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

}  // namespace

TEST(propagation, correctness_no_dummy) {
  util::random::Generator gen(239);
  core::sat::Problem problem(IPS_PROJECT_ROOT
                             "/resources/cnf/common/unsat_pancake_vs_selection_7_4-@2.cnf");

  core::sat::prop::SimpProp simp_prop;
  core::sat::prop::Prop& sat_solver = simp_prop;
  core::sat::solver::MapleCOMSPSSolver mcomsps_solver;
  mcomsps_solver.load_problem(problem);
  simp_prop.load_problem(problem);

  int num_tests = 100000;
  while (num_tests--) {
    std::set<int> vars;
    int size = util::random::sample<int>(0, 20);
    Mini::vec<Mini::Lit> ms_assumption(size);
    for (int i = 0; i < size; ++i) {
      int var = util::random::sample<int>(0, simp_prop.num_vars() - 1);
      while (vars.find(var) != vars.end()) {
        var = util::random::sample<int>(0, simp_prop.num_vars() - 1);
      }
      int sign = util::random::sample<int>(0, 1);
      ms_assumption[i] = Mini::mkLit(var, sign);
    }

    Mini::vec<Mini::Lit> dummy;
    bool a = sat_solver.propagate(ms_assumption, dummy);
    bool b = sat_solver.propagate(ms_assumption);
    ASSERT_EQ(a, b);
  }
}

TEST(propagation, correctness_subtree) {
  util::random::Generator gen(239);
  std::filesystem::path cnf_path(IPS_PROJECT_ROOT
                                 "/resources/cnf/common/unsat_pancake_vs_selection_7_4-@2.cnf");
  Minisat::SimpSolver solver;

  {
    util::GzFile gz_file(cnf_path);
    solver.parsing = true;
    Minisat::parse_DIMACS(gz_file.native_handle(), solver);
    solver.parsing = false;
    solver.eliminate(true);
  }

  int num_tests = 1000;
  while (num_tests--) {
    std::set<int> vars;
    int size = util::random::sample<int>(0, 20);
    int head_size = util::random::sample<int>(0, size);
    Mini::vec<Mini::Lit> ms_assumption(size);
    for (int i = 0; i < size; ++i) {
      int var = util::random::sample<int>(0, solver.nVars() - 1);
      while (vars.find(var) != vars.end()) {
        var = util::random::sample<int>(0, solver.nVars() - 1);
      }
      int sign = util::random::sample<int>(0, 1);
      ms_assumption[i] = Mini::mkLit(var, sign);
    }

    uint64_t fast = solver.prop_check_subtree(ms_assumption, head_size);
    uint64_t naive = num_conflicts_in_subtree(solver, ms_assumption, head_size);
    ASSERT_EQ(fast, naive);
  }

  core::TimeTracer::print_summary(10);
}
