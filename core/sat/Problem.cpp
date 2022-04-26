#include "core/sat/Problem.h"

#include <utility>

#include "util/TimeTracer.h"
#include "util/mini.h"
#include "core/sat/solver/sequential/SimpSolver.h"

namespace core::sat {

core::sat::State Problem::read_clauses() {
  _clauses.clear();
  util::GzFile gz_file(_path);
  _solver.parsing = true;
  Minisat::parse_DIMACS(gz_file.native_handle(), _solver);
  _solver.parsing = false;
  bool ok = true;

  if (_eliminate) {
    ok = IPS_BLOCK_R(problem_eliminate, _solver.eliminate(true));
  }

  _solver.toDimacs(_clauses, _mapping);

  if (_clauses.empty()) {
    if (ok) {
      _solver.solve();
      _model = _solver.model;
      return core::sat::SAT;
    } else {
      return core::sat::UNSAT;
    }
  } else {
    return core::sat::UNKNOWN;
  }
}

Problem::Problem(std::filesystem::path path, bool eliminate) : _path(std::move(path)), _eliminate(eliminate) {
  _result = IPS_BLOCK_R(problem_construct, read_clauses());
}

Mini::vec<Mini::Var> const& Problem::get_mapping() const noexcept { return _mapping; }

Mini::vec<Mini::lbool> Problem::remap_variables(Mini::vec<Mini::lbool> const& vars) {
  _solver.model.growTo(_solver.nVars(), Minisat::l_Undef);
  for (int i = 0; i < _solver.nVars(); ++i) {
    if (_solver.value(i) != Mini::l_Undef) {
      _solver.model[i] = _solver.value(i);
    } else {
      if (_mapping.size() <= i || _mapping[i] == -1) {
        continue;
      }
      IPS_VERIFY(0 <= _mapping[i] && _mapping[i] < vars.size());
      _solver.model[i] = vars[_mapping[i]];
    }
  }
  _solver.extendModel();
  return _solver.model;
}

Mini::vec<Mini::lbool> const& Problem::get_model() const noexcept { return _model; }

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
  if (&o != this) {
    std::lock_guard<std::mutex> lg(o._load_mutex);
    _clauses = o._clauses;
    _eliminate = o._eliminate;
    _path = o._path;
    _result = o._result;
  }
  return *this;
}

Problem& Problem::operator=(Problem&& o) {
  if (&o != this) {
    std::lock_guard<std::mutex> lg(o._load_mutex);
    _clauses = std::move(o._clauses);
    _eliminate = o._eliminate;
    _path = std::move(o._path);
    _result = o._result;
  }
  return *this;
}

bool Problem::operator<(Problem const& o) const noexcept { return _path < o._path; }

std::filesystem::path const& Problem::path() const noexcept { return _path; }

std::vector<lit_vec_t> const& Problem::clauses() const noexcept { return _clauses; }

State Problem::get_result() const noexcept { return _result; }

std::string Problem::to_string() const noexcept {
  std::stringstream os;
  os << (_eliminate ? "[elm]:" : "[raw]:") << _path.parent_path().filename().string() << "/"
     << _path.filename().string();
  return os.str();
}

}  // namespace core::sat

std::ostream& operator<<(std::ostream& os, core::sat::Problem const& problem) { return os << problem.to_string(); }

#undef MINI_LIT
