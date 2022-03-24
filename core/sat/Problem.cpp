#include "core/sat/Problem.h"

#include <utility>

namespace {

using namespace std;

core::sat::State read_clauses_to(
    std::filesystem::path const& path,
    std::vector<Mini::vec<Mini::Lit>>& clauses, bool eliminate) {
  clauses.clear();
  Minisat::SimpSolver solver;
  util::GzFile gz_file(path);
  solver.parsing = true;
  Minisat::parse_DIMACS(gz_file.native_handle(), solver);
  solver.parsing = false;
  bool ok = true;
  if (eliminate) {
    ok = solver.eliminate(true);
  }
  solver.toDimacs(clauses);

  if (clauses.empty()) {
    return ok ? core::sat::SAT : core::sat::UNSAT;
  } else {
    return core::sat::UNKNOWN;
  }
}

}  // namespace

namespace core::sat {

Problem::Problem(std::filesystem::path path, bool eliminate)
    : _path(std::move(path)), _eliminate(eliminate) {
  _result = read_clauses_to(_path, _clauses, _eliminate);
}

std::filesystem::path const& Problem::path() const noexcept {
  return _path;
}

std::vector<Mini::vec<Mini::Lit>> const& Problem::clauses() const noexcept {
  return _clauses;
}

State Problem::get_result() const noexcept {
  return _result;
}

}  // namespace core::sat