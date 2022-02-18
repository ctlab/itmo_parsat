#include "core/sat/Problem.h"

#include "core/sat/minisat/minisat/simp/SimpSolver.h"
#include "minisat/core/Dimacs.h"

namespace {

void read_clauses_to(std::filesystem::path const& path, std::vector<std::vector<int>>& clauses) {
  util::GzFile file(path);
  Minisat::SimpSolver solver;
  {
    util::GzFile gz_file(path);
    solver.parsing = true;
    Minisat::parse_DIMACS(gz_file.native_handle(), solver, true);
    solver.parsing = false;
  }
  IPS_INFO("Performing elimination.");
  solver.eliminate(true);
  clauses.clear();
  for (auto it = solver.clausesBegin(); it != solver.clausesEnd(); ++it) {
    auto const& clause = *it;
    std::vector<int> clause_vec(clause.size());
    for (int i = 0; i < clause.size(); ++i) {
      clause_vec[i] = INT_LIT(clause[i]);
    }
    clauses.push_back(std::move(clause_vec));
  }
  IPS_INFO("Elimination performed, clauses saved.");
}

}  // namespace

namespace core::sat {

Problem::Problem(std::filesystem::path const& path) : _path(path) {
  read_clauses_to(path, _clauses);
}

std::filesystem::path const& Problem::path() const noexcept {
  return _path;
}

std::vector<std::vector<int>> const& Problem::clauses() const noexcept {
  return _clauses;
}

}  // namespace core::sat