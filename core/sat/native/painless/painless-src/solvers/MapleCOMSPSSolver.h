// -----------------------------------------------------------------------------
// Copyright (C) 2017  Ludovic LE FRIOUX
//
// This file is part of PaInleSS.
//
// PaInleSS is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <http://www.gnu.org/licenses/>.
// -----------------------------------------------------------------------------

#pragma once

#include "../clauses/ClauseBuffer.h"
#include "../solvers/SolverInterface.h"
#include "../utils/Threading.h"

#include "core/sat/native/mini/mapleCOMSPS/mapleCOMSPS/simp/SimpSolver.h"

namespace painless {

/// Instance of a MapleCOMSPS solver
class MapleCOMSPSSolver : public SolverInterface, public MapleCOMSPS::SimpSolver {
 public:
  /// Load formula from a given dimacs file, return false if failed.
  bool loadFormula(const char* filename);

  bool loadFormula(std::vector<Mini::vec<Mini::Lit>> const& clauses);

  /// Get the number of variables of the current resolution.
  int getVariablesCount();

  /// Get a variable suitable for search splitting.
  int getDivisionVariable();

  /// Set initial phase for a given variable.
  void setPhase(const int var, const bool phase);

  /// Bump activity of a given variable.
  void bumpVariableActivity(const int var, const int times);

  /// Interrupt resolution, solving cannot continue until interrupt is unset.
  void setSolverInterrupt();

  /// Remove the SAT solving interrupt request.
  void unsetSolverInterrupt();

  /// Solve the formula with a given cube.
  PSatResult solve(Mini::vec<Mini::Lit> const& assumptions, const std::vector<int>& cube);

  /// Add a permanent clause to the formula.
  void addClause(ClauseExchange* clause);

  /// Add a list of permanent clauses to the formula.
  void addClauses(const std::vector<ClauseExchange*>& clauses);

  /// Add a list of initial clauses to the formula.
  void addInitialClauses(const std::vector<ClauseExchange*>& clauses);

  /// Add a learned clause to the formula.
  void addLearnedClause(ClauseExchange* clause);

  /// Add a list of learned clauses to the formula.
  void addLearnedClauses(const std::vector<ClauseExchange*>& clauses);

  /// Get a list of learned clauses.
  void getLearnedClauses(std::vector<ClauseExchange*>& clauses);

  /// Request the solver to produce more clauses.
  void increaseClauseProduction();

  /// Request the solver to produce less clauses.
  void decreaseClauseProduction();

  /// Get solver statistics.
  SolvingStatistics getStatistics();

  /// Return the model in case of SAT result.
  vector<int> getModel();

  /// Native diversification.
  void diversify(int id);

  /// Constructor.
  MapleCOMSPSSolver(int id, int lbd_limit);

  /// Copy constructor.
  MapleCOMSPSSolver(int id, int lbd_limit, const MapleCOMSPSSolver& other);

  /// Destructor.
  ~MapleCOMSPSSolver() noexcept = default;

  vector<int> getFinalAnalysis();

  vector<int> getSatAssumptions();

 public:
  /// Size limit used to share clauses.
  atomic<int> lbdLimit;

 protected:
  ClauseBuffer clausesToImport;
  ClauseBuffer unitsToImport;
  ClauseBuffer clausesToExport;

  /// Buffer used to add permanent clauses.
  ClauseBuffer clausesToAdd;

  /// Used to stop or continue the resolution.
  std::atomic_bool stopSolver{false};

  /// Callback to export/import clauses.
  friend MapleCOMSPS::Lit cbkMapleCOMSPSImportUnit(void*);
  friend bool cbkMapleCOMSPSImportClause(void*, int*, MapleCOMSPS::vec<MapleCOMSPS::Lit>&);
  friend void cbkMapleCOMSPSExportClause(void*, int, MapleCOMSPS::vec<MapleCOMSPS::Lit>&);
};

}  // namespace painless