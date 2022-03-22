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

// MapleCOMSPS includes
#include "core/sat/native/mini/utils/System.h"
#include "core/sat/native/mini/mapleCOMSPS/mapleCOMSPS/core/Dimacs.h"

#include "../clauses/ClauseManager.h"
#include "../solvers/Reducer.h"

namespace painless {

using namespace MapleCOMSPS;
using namespace Mini;

// Macros for minisat literal representation conversion
#define MINI_LIT(lit) lit > 0 ? mkLit(lit - 1, false) : mkLit((-lit) - 1, true)
#define INT_LIT(lit) sign(lit) ? -(var(lit) + 1) : (var(lit) + 1)

static void makeMiniVec(ClauseExchange* cls, vec<Lit>& mcls) {
  for (size_t i = 0; i < cls->size; i++) {
    mcls.push(MINI_LIT(cls->lits[i]));
  }
}

Reducer::Reducer(int id, SolverInterface* _solver) : SolverInterface(id) {
  solver = _solver;
}

Reducer::~Reducer() {
  delete solver;
}

bool Reducer::loadFormula(const char* filename) {
  return solver->loadFormula(filename);
}

// Get the number of variables of the formula
int Reducer::getVariablesCount() {
  return solver->getVariablesCount();
}

// Get a variable suitable for search splitting
int Reducer::getDivisionVariable() {
  return solver->getDivisionVariable();
}

// Set initial phase for a given variable
void Reducer::setPhase(const int var, const bool phase) {
  solver->setPhase(var, phase);
}

// Bump activity for a given variable
void Reducer::bumpVariableActivity(const int var, const int times) {
  solver->bumpVariableActivity(var, times);
}

// Interrupt the SAT solving, so it can be started again with new assumptions
void Reducer::setSolverInterrupt() {
  _interrupt = true;
  solver->setSolverInterrupt();
}

void Reducer::unsetSolverInterrupt() {
  _interrupt = false;
  solver->unsetSolverInterrupt();
}

// Diversify the solver
void Reducer::diversify(int id) {
  solver->diversify(id);
}

// Solve the formula with a given set of assumptions
// return 10 for SAT, 20 for UNSAT, 0 for UNKNOWN
PSatResult Reducer::solve(
    Mini::vec<Mini::Lit> const& assumptions, const vector<int>&) {
  this->assumptions = assumptions;

  while (!_interrupt) {
    ClauseExchange* cls = nullptr;
    ClauseExchange* strengthenedCls = nullptr;
    if (!clausesToImport.getClause(&cls)) {
      continue;
    }
    if (strengthed(cls, &strengthenedCls)) {
      if (strengthenedCls->size == 0) {
        ClauseManager::releaseClause(cls);
        ClauseManager::releaseClause(strengthenedCls);
        return PUNSAT;
      }
      clausesToExport.addClause(strengthenedCls);
    }
    ClauseManager::releaseClause(cls);
  }
  return PUNKNOWN;
}

bool Reducer::strengthed(ClauseExchange* cls, ClauseExchange** outCls) {
  vector<int> assumps;
  vector<int> tmpNewClause;
  for (size_t ind = 0; ind < cls->size; ind++) {
    assumps.push_back(-cls->lits[ind]);
  }
  PSatResult res = solver->solve(assumptions, assumps);
  if (res == PUNSAT) {
    tmpNewClause = solver->getFinalAnalysis();
  } else if (res == PSAT) {
    tmpNewClause = solver->getSatAssumptions();
  }

  if (tmpNewClause.size() < cls->size || res == PSAT) {
    *outCls = ClauseManager::allocClause(tmpNewClause.size());
    for (int idLit = 0; idLit < tmpNewClause.size(); idLit++) {
      (*outCls)->lits[idLit] = tmpNewClause[idLit];
    }
    (*outCls)->lbd = cls->lbd;
    if ((*outCls)->size < (*outCls)->lbd) {
      (*outCls)->lbd = (*outCls)->size;
    }
    if (res == PSAT) {
      solver->addClause(*outCls);
    }
  }
  return tmpNewClause.size() < cls->size;
}

void Reducer::addClause(ClauseExchange* clause) {
  solver->addClause(clause);
}

void Reducer::addLearnedClause(ClauseExchange* clause) {
  if (clause->size == 1) {
    solver->addLearnedClause(clause);
  } else {
    clausesToImport.addClause(clause);
  }
}

void Reducer::addClauses(const vector<ClauseExchange*>& clauses) {
  solver->addClauses(clauses);
}

void Reducer::addInitialClauses(const vector<ClauseExchange*>& clauses) {
  solver->addInitialClauses(clauses);
}

void Reducer::addLearnedClauses(const vector<ClauseExchange*>& clauses) {
  for (size_t i = 0; i < clauses.size(); i++) {
    addLearnedClause(clauses[i]);
  }
}

void Reducer::getLearnedClauses(vector<ClauseExchange*>& clauses) {
  clausesToExport.getClauses(clauses);
}

void Reducer::increaseClauseProduction() {
  solver->increaseClauseProduction();
}

void Reducer::decreaseClauseProduction() {
  solver->decreaseClauseProduction();
}

SolvingStatistics Reducer::getStatistics() {
  return solver->getStatistics();
}

vector<int> Reducer::getModel() {
  return solver->getModel();
}

vector<int> Reducer::getFinalAnalysis() {
  return solver->getFinalAnalysis();
}

vector<int> Reducer::getSatAssumptions() {
  return solver->getSatAssumptions();
}

}  // namespace painless