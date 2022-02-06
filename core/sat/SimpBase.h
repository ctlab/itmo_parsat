#ifndef ITMO_PARSAT_SIMPBASE_H
#define ITMO_PARSAT_SIMPBASE_H

#include <filesystem>

#include "minisat/simp/SimpSolver.h"
#include "minisat/core/Dimacs.h"
#include "core/proto/solve_config.pb.h"
#include "core/util/Generator.h"
#include "core/util/GzFile.h"

namespace core::sat {

class SimpBase : public Minisat::SimpSolver {
 public:
  SimpBase() = default;

  SimpBase(SimpSolverConfig const& config);

  void parse_cnf(std::filesystem::path const& path);

  [[nodiscard]] uint32_t num_vars() const noexcept;

 protected:
  bool _propagate_confl(Minisat::vec<Minisat::Lit> const& assumptions);
};

}  // namespace core::sat

#endif  // ITMO_PARSAT_SIMPBASE_H
