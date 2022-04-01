#ifndef ITMO_PARSAT_PROBLEM_H
#define ITMO_PARSAT_PROBLEM_H

#include <fstream>
#include <filesystem>
#include <vector>
#include <mutex>

#include "util/Logger.h"
#include "util/GzFile.h"

#include "core/types.h"
#include "core/sat/native/mini/minisat/minisat/core/Dimacs.h"
#include "core/sat/native/mini/minisat/minisat/simp/SimpSolver.h"

namespace core::sat {

enum State {
  SAT,
  UNSAT,
  UNKNOWN,
};

/**
 * @brief The problem descriptor class.
 */
class Problem {
 public:
  /**
   * @brief Constructs the problem by the cnf path.
   * @param path the path to cnf formula
   */
  explicit Problem(std::filesystem::path path, bool eliminate = true);

  Problem(Problem const& o);

  Problem& operator=(Problem const& o);

  [[nodiscard]] std::filesystem::path const& path() const noexcept;

  [[nodiscard]] std::vector<Mini::vec<Mini::Lit>> const& clauses()
      const noexcept;

  [[nodiscard]] State get_result() const noexcept;

  std::string to_string() const noexcept;

  bool operator<(Problem const& o) const noexcept;

 private:
  /// @note mutable for lazy loading.
  mutable std::mutex _load_mutex;
  mutable std::vector<lit_vec_t> _clauses;
  bool _eliminate;
  std::filesystem::path _path;
  State _result;
};

}  // namespace core::sat

std::ostream& operator<<(std::ostream& os, core::sat::Problem const& problem);

#endif  // ITMO_PARSAT_PROBLEM_H
