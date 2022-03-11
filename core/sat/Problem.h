#ifndef ITMO_PARSAT_PROBLEM_H
#define ITMO_PARSAT_PROBLEM_H

#include <fstream>
#include <filesystem>
#include <vector>
#include <mutex>

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
  /// @note mutable for lazy loading.
  mutable std::mutex _load_mutex;
  mutable std::vector<std::vector<int>> _clauses;
  std::filesystem::path _path;
};

}  // namespace core::sat

//#define MINI_LIT(lit) lit > 0 ? MS_NS::mkLit(lit - 1, false) : MS_NS::mkLit((-lit) - 1, true)
//#define INT_LIT(lit) MS_NS::sign(lit) ? -(MS_NS::var(lit) + 1) : (MS_NS::var(lit) + 1)

#endif  // ITMO_PARSAT_PROBLEM_H
