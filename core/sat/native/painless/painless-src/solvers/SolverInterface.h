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

#include "../clauses/ClauseExchange.h"

#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "core/sat/native/mini/mtl/Vec.h"
#include "core/sat/native/mini/utils/Lit.h"

#define ID_SYM 0
#define ID_XOR 1

using namespace std;

namespace painless {

enum PSatResult { PSAT = 10, PUNSAT = 20, PUNKNOWN = 0 };

struct SolvingStatistics {
  unsigned long propagations = 0;
  unsigned long decisions = 0;
  unsigned long conflicts = 0;
  unsigned long restarts = 0;
  double memPeak = 0;
};

class SolverInterface {
 public:
  virtual bool loadFormula(const char* filename) = 0;

  virtual bool loadFormula(
      std::vector<Mini::vec<Mini::Lit>> const& clauses) = 0;

  virtual int getVariablesCount() = 0;

  virtual int getDivisionVariable() = 0;

  virtual void setPhase(int var, bool phase) = 0;

  virtual void bumpVariableActivity(int var, int times) = 0;

  virtual void setSolverInterrupt() = 0;

  virtual void unsetSolverInterrupt() = 0;

  virtual PSatResult solve(
      Mini::vec<Mini::Lit> const& assumptions, const vector<int>& cube) = 0;

  virtual void addClause(ClauseExchange* clause) = 0;

  virtual void addClauses(const vector<ClauseExchange*>& clauses) = 0;

  virtual void addInitialClauses(const vector<ClauseExchange*>& clauses) = 0;

  virtual void addLearnedClause(ClauseExchange* clauses) = 0;

  virtual void addLearnedClauses(const vector<ClauseExchange*>& clauses) = 0;

  virtual void getLearnedClauses(vector<ClauseExchange*>& clauses) = 0;

  virtual void increaseClauseProduction() = 0;

  virtual void decreaseClauseProduction() = 0;

  virtual SolvingStatistics getStatistics() = 0;

  virtual vector<int> getModel() = 0;

  virtual void diversify(int id) = 0;

  virtual vector<int> getFinalAnalysis() = 0;

  virtual vector<int> getSatAssumptions() = 0;

  virtual ~SolverInterface() noexcept = default;

  SolverInterface();

 public:
  int id;
};

}  // namespace painless