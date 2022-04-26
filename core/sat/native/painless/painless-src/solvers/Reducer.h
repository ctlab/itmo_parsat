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

namespace painless {

class Reducer : public SolverInterface {
 public:
  bool loadFormula(const char* filename) override;

  bool loadFormula(std::vector<Mini::vec<Mini::Lit>> const& clauses) override;

  int getVariablesCount() override;

  int getDivisionVariable() override;

  void setPhase(int var, bool phase) override;

  void bumpVariableActivity(int var, int times) override;

  void setSolverInterrupt() override;

  void unsetSolverInterrupt() override;

  PSatResult solve(
      Mini::vec<Mini::Lit> const& assumptions,
      const std::vector<int>& cube) override;

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

  vector<int> getFinalAnalysis() override;

  void diversify(int id) override;

  vector<int> getSatAssumptions() override;

  explicit Reducer(SolverInterface* solver);

  ~Reducer() noexcept override;

  bool strengthed(ClauseExchange* cls, ClauseExchange** outCls);

 protected:
  SolverInterface* solver;
  ClauseBuffer clausesToImport;
  ClauseBuffer clausesToExport;
  Mini::vec<Mini::Lit> assumptions;

  bool _stop = false;
  bool _interrupt = false;
};

}  // namespace painless
