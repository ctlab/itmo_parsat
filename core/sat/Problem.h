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
   * @param path the path to cnf formula in dimacs format
   */
  explicit Problem(std::filesystem::path path, bool eliminate = true);

  Problem(Problem const& o);
  Problem(Problem&& o);

  Problem& operator=(Problem const& o);
  Problem& operator=(Problem&& o);

  /**
   * @return the path to cnf formula specified in the constructor
   */
  [[nodiscard]] std::filesystem::path const& path() const noexcept;

  /**
   * @return the set of clauses describing the formula
   */
  [[nodiscard]] std::vector<lit_vec_t> const& clauses() const noexcept;

  /**
   * @return the result of parsing
   */
  [[nodiscard]] State get_result() const noexcept;

  /**
   * @return string representation of formula, including path and elimination
   */
  std::string to_string() const noexcept;

  /*
   * @brief Remaps variables and extends model for source variables
   * */
  [[nodiscard]] Mini::vec<Mini::lbool> remap_and_extend_model(Mini::vec<Mini::lbool> const& model);

  /*
   * @brief Remaps variables
   * */
  [[nodiscard]] std::vector<int> remap_variables(std::vector<int> const& vars) const;

  /**
   * @return The satisfying assignment if the problem has been solved on parsing stage and is SAT
   */
  [[nodiscard]] Mini::vec<Mini::lbool> const& get_model() const noexcept;

  Mini::vec<Mini::Var> const& get_mapping() const noexcept;

  std::vector<int> const& get_rev_mapping() const noexcept;

  bool operator<(Problem const& o) const noexcept;

 private:
  core::sat::State read_clauses();

  mutable std::mutex _load_mutex;
  std::vector<lit_vec_t> _clauses;
  Mini::vec<Mini::Var> _mapping;
  std::vector<int> _rev_mapping;
  Mini::vec<Mini::lbool> _model;

  bool _eliminate;
  std::filesystem::path _path;
  Minisat::SimpSolver _solver;
  State _result;
};

}  // namespace core::sat

std::ostream& operator<<(std::ostream& os, core::sat::Problem const& problem);

#endif  // ITMO_PARSAT_PROBLEM_H
