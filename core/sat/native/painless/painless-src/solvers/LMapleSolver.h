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

#include "core/sat/native/mini/lmaple/lmaple/core/SolverTypes.h"
#include "core/sat/native/mini/lmaple/lmaple/simp/SimpSolver.h"
#include "core/sat/native/mini/mtl/Vec.h"

namespace painless {

class LMapleSolver : public SolverInterface, public LMaple::SimpSolver {
 public:
  bool loadFormula(const char* filename) override;

  bool loadFormula(std::vector<Mini::vec<Mini::Lit>> const& clauses) override;

  int getVariablesCount() override;

  int getDivisionVariable() override;

  void setPhase(int var, bool phase) override;

  void bumpVariableActivity(int var, int times) override;

  void setSolverInterrupt() override;

  void unsetSolverInterrupt() override;

  PSatResult solve(Mini::vec<Mini::Lit> const& assumptions, const std::vector<int>& cube) override;

  void addClause(ClauseExchange* clause) override;

  void addClauses(const std::vector<ClauseExchange*>& clauses) override;

  void addInitialClauses(const std::vector<ClauseExchange*>& clauses) override;

  void addLearnedClause(ClauseExchange* clause) override;

  void addLearnedClauses(const std::vector<ClauseExchange*>& clauses) override;

  void getLearnedClauses(std::vector<ClauseExchange*>& clauses) override;

  void increaseClauseProduction() override;

  void decreaseClauseProduction() override;

  SolvingStatistics getStatistics() override;

  Mini::vec<Mini::lbool> getModel() const override;

  void diversify(int id) override;

  explicit LMapleSolver(int lbd_limit = 2, bool sharing_clauses = true);

  ~LMapleSolver() noexcept override = default;

  vector<int> getFinalAnalysis() override;

  vector<int> getSatAssumptions() override;

 public:
  atomic<int> lbdLimit;

  ClauseBuffer clausesToImport;
  ClauseBuffer unitsToImport;
  ClauseBuffer clausesToExport;
  ClauseBuffer clausesToAdd;

  friend LMaple::Lit cbkMapleCOMSPSImportUnit(void*);
  friend bool cbkMapleCOMSPSImportClause(void*, int*, LMaple::vec<LMaple::Lit>&);
  friend void cbkMapleCOMSPSExportClause(void*, int, LMaple::vec<LMaple::Lit>&);
};

}  // namespace painless
