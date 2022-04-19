#include "core/sat/Problem.h"

#include <utility>

#include "util/TimeTracer.h"

namespace {

using namespace std;

core::sat::State read_clauses_to(
    std::filesystem::path const& path, std::vector<Mini::vec<Mini::Lit>>& clauses, bool eliminate) {
  clauses.clear();
  Minisat::SimpSolver solver;
  util::GzFile gz_file(path);
  solver.parsing = true;
  Minisat::parse_DIMACS(gz_file.native_handle(), solver);
  solver.parsing = false;
  bool ok = true;
  if (eliminate) { ok = IPS_TRACE_N_V("Problem::eliminate", solver.eliminate(true)); }
  solver.toDimacs(clauses);

  if (clauses.empty()) {
    return ok ? core::sat::SAT : core::sat::UNSAT;
  } else {
    return core::sat::UNKNOWN;
  }
}

}  // namespace

namespace core::sat {

Problem::Problem(std::filesystem::path path, bool eliminate) : _path(std::move(path)), _eliminate(eliminate) {
  _result = IPS_TRACE_N_V("Problem::Problem", read_clauses_to(_path, _clauses, _eliminate));
}

Problem::Problem(Problem const& o) {
  std::lock_guard<std::mutex> lg(o._load_mutex);
  _clauses = o._clauses;
  _eliminate = o._eliminate;
  _path = o._path;
  _result = o._result;
}

Problem::Problem(Problem&& o) {
  std::lock_guard<std::mutex> lg(o._load_mutex);
  _clauses = std::move(o._clauses);
  _eliminate = o._eliminate;
  _path = std::move(o._path);
  _result = o._result;
}

Problem& Problem::operator=(Problem const& o) {
  std::lock_guard<std::mutex> lg(o._load_mutex);
  _clauses = o._clauses;
  _eliminate = o._eliminate;
  _path = o._path;
  _result = o._result;
  return *this;
}

Problem& Problem::operator=(Problem&& o) {
  std::lock_guard<std::mutex> lg(o._load_mutex);
  _clauses = std::move(o._clauses);
  _eliminate = o._eliminate;
  _path = std::move(o._path);
  _result = o._result;
  return *this;
}

bool Problem::operator<(Problem const& o) const noexcept {
  return _path < o._path;
}

std::filesystem::path const& Problem::path() const noexcept {
  return _path;
}

std::vector<lit_vec_t> const& Problem::clauses() const noexcept {
  return _clauses;
}

State Problem::get_result() const noexcept {
  return _result;
}

std::string Problem::to_string() const noexcept {
  std::stringstream os;
  os << (_eliminate ? "[elm]:" : "[raw]:") << _path.parent_path().filename().string() << "/"
     << _path.filename().string();
  return os.str();
}

}  // namespace core::sat

std::ostream& operator<<(std::ostream& os, core::sat::Problem const& problem) {
  return os << problem.to_string();
}
