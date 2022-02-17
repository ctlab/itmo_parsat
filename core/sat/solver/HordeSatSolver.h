#ifndef ITMO_PARSAT_HORDESATSOLVER_H
#define ITMO_PARSAT_HORDESATSOLVER_H

#include <vector>
#include <memory>
#include <mpi/mpi.h>
#include <mutex>
#include <condition_variable>

#include "core/sat/solver/Solver.h"
#include "core/sat/prop/SimpProp.h"
#include "core/util/Registry.h"

#include "core/sat/hordesat/hordesat-src/solvers/MiniSat.h"
#include "core/sat/hordesat/hordesat-src/solvers/Lingeling.h"
#include "core/sat/hordesat/hordesat-src/utilities/DebugUtils.h"
#include "core/sat/hordesat/hordesat-src/utilities/Logger.h"
#include "core/sat/hordesat/hordesat-src/utilities/Threading.h"
#include "core/sat/hordesat/hordesat-src/utilities/ParameterProcessor.h"
#include "core/sat/hordesat/hordesat-src/sharing/AllToAllSharingManager.h"
#include "core/sat/hordesat/hordesat-src/sharing/LogSharingManager.h"

namespace core::sat {

class HordeSatSolver : public Solver {
 public:
  explicit HordeSatSolver(HordeSatSolverConfig const& config);

  ~HordeSatSolver() noexcept override;

  void parse_cnf(std::filesystem::path const& path) override;

  State solve(Minisat::vec<Minisat::Lit> const& assumptions) override;

  [[nodiscard]] unsigned num_vars() const noexcept override;

  [[nodiscard]] bool propagate_confl(Minisat::vec<Minisat::Lit> const& assumptions) override;

 private:
  void _diversify();

 private:
  uint32_t _num_solvers;
  std::vector<std::unique_ptr<PortfolioSolverInterface>> _solvers;
  std::vector<PortfolioSolverInterface*> _solver_ptrs;
  std::unique_ptr<SharingManagerInterface> _sharing_manager;
  std::unique_ptr<int> _ending_buffer;
  std::vector<std::thread> _solver_threads;
  State _result;
  int mpi_size, mpi_rank;

  bool _solvingDoneLocal = false;
  Mutex _interruptLock;

  std::mutex _slv_mutex;
  std::condition_variable _slv_cv;
  std::vector<int> _assumptions;
  uint64_t _solve_idx = 0;
  bool _stop = false;

  prop::SimpProp _prop;
  HordeSatSolverConfig _cfg;
};

}  // namespace core::sat

#endif  // ITMO_PARSAT_HORDESATSOLVER_H
