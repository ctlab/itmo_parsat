#ifndef ITMO_PARSAT_PROBLEM_H
#define ITMO_PARSAT_PROBLEM_H

#include <fstream>
#include <filesystem>
#include <vector>

#include "util/Logger.h"
#include "util/GzFile.h"

namespace core::sat {

enum State {
  SAT,
  UNSAT,
  UNKNOWN,
};

class Problem {
 public:
  explicit Problem(std::filesystem::path const& path);

  [[nodiscard]] std::filesystem::path const& path() const noexcept;

  [[nodiscard]] std::vector<std::vector<int>> const& clauses() const noexcept;

 private:
  std::filesystem::path _path;
  std::vector<std::vector<int>> _clauses;
};

}  // namespace core::sat

#define MINI_LIT(lit) lit > 0 ? Minisat::mkLit(lit - 1, false) : Minisat::mkLit((-lit) - 1, true)
#define INT_LIT(lit) Minisat::sign(lit) ? -(Minisat::var(lit) + 1) : (Minisat::var(lit) + 1)

#endif  // ITMO_PARSAT_PROBLEM_H
