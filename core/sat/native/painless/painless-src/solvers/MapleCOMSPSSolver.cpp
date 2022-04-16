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
#include "core/sat/native/mini/mapleCOMSPS/mapleCOMSPS/simp/SimpSolver.h"

#include "../clauses/ClauseManager.h"
#include "../solvers/MapleCOMSPSSolver.h"

#include <unordered_set>

using namespace MapleCOMSPS;
using namespace Mini;

// Macros for minisat literal representation conversion
#define MINI_LIT(lit) lit > 0 ? mkLit(lit - 1, false) : mkLit((-lit) - 1, true)

#define INT_LIT(lit) sign(lit) ? -(var(lit) + 1) : (var(lit) + 1)

namespace painless {

static void makeMiniVec(ClauseExchange* cls, vec<Lit>& mcls) {
  for (size_t i = 0; i < cls->size; i++) { mcls.push(MINI_LIT(cls->lits[i])); }
}

void cbkMapleCOMSPSExportClause(void* issuer, int lbd, vec<Lit>& cls) {
  auto* mp = (MapleCOMSPSSolver*) issuer;
  if (lbd > mp->lbdLimit) return;

  ClauseExchange* ncls = ClauseManager::allocClause(cls.size());
  for (int i = 0; i < cls.size(); i++) { ncls->lits[i] = INT_LIT(cls[i]); }
  ncls->lbd = lbd;

  mp->clausesToExport.addClause(ncls);
}

Lit cbkMapleCOMSPSImportUnit(void* issuer) {
  auto* mp = (MapleCOMSPSSolver*) issuer;
  Lit l = lit_Undef;
  ClauseExchange* cls = nullptr;
  if (!mp->unitsToImport.getClause(&cls)) return l;
  l = MINI_LIT(cls->lits[0]);

  ClauseManager::releaseClause(cls);
  return l;
}

bool cbkMapleCOMSPSImportClause(void* issuer, int* lbd, vec<Lit>& mcls) {
  auto* mp = (MapleCOMSPSSolver*) issuer;
  ClauseExchange* cls = nullptr;
  if (!mp->clausesToImport.getClause(&cls)) return false;

  makeMiniVec(cls, mcls);
  *lbd = cls->lbd;

  ClauseManager::releaseClause(cls);
  return true;
}

MapleCOMSPSSolver::MapleCOMSPSSolver(int lbd_limit, bool sharing_clauses) : lbdLimit(lbd_limit) {
  if (sharing_clauses) {
    cbkExportClause = cbkMapleCOMSPSExportClause;
    cbkImportClause = cbkMapleCOMSPSImportClause;
    cbkImportUnit = cbkMapleCOMSPSImportUnit;
    issuer = this;
  }
}

bool MapleCOMSPSSolver::loadFormula(const char* filename) {
  gzFile in = gzopen(filename, "rb");
  parse_DIMACS(in, *this);
  gzclose(in);
  return true;
}

bool MapleCOMSPSSolver::loadFormula(std::vector<Mini::vec<Mini::Lit>> const& clauses) {
  parsing = true;
  MapleCOMSPS::SimpSolver::loadClauses(clauses);
  parsing = false;
  return true;
}

int MapleCOMSPSSolver::getVariablesCount() {
  return nVars();
}

int MapleCOMSPSSolver::getDivisionVariable() {
  return (rand() % getVariablesCount()) + 1;
}

void MapleCOMSPSSolver::setPhase(const int var, const bool phase) {
  setPolarity(var - 1, phase);
}

void MapleCOMSPSSolver::bumpVariableActivity(const int var, const int times) {}

void MapleCOMSPSSolver::setSolverInterrupt() {
  interrupt();
}

void MapleCOMSPSSolver::unsetSolverInterrupt() {
  clearInterrupt();
}

void MapleCOMSPSSolver::diversify(int id) {
  if (id == ID_XOR) {
    GE = true;
  } else {
    GE = false;
  }

  if (id % 2) {
    VSIDS = false;
  } else {
    VSIDS = true;
  }

  if (id % 4 >= 2) {
    verso = false;
  } else {
    verso = true;
  }
}

PSatResult MapleCOMSPSSolver::solve(
    Mini::vec<Mini::Lit> const& assumptions, const vector<int>& cube) {
  unsetSolverInterrupt();
  PSatResult result = PUNKNOWN;

  vector<ClauseExchange*> tmp;
  clausesToAdd.getClauses(tmp);

  for (size_t ind = 0; ind < tmp.size(); ind++) {
    vec<Lit> mcls;
    makeMiniVec(tmp[ind], mcls);
    if (!SimpSolver::addClause(mcls)) {
      result = PUNSAT;
      break;
    }
  }

  if (result == PUNKNOWN) {
    std::unordered_set<int> present;
    vec<Lit> miniAssumptions;
    for (int i = 0; i < assumptions.size(); ++i) {
      if (present.count(assumptions[i].x) == 0) {
        miniAssumptions.push(assumptions[i]);
        present.insert(assumptions[i].x);
      }
    }
    for (int ind : cube) { miniAssumptions.push(MINI_LIT(ind)); }

    lbool res = solveLimited(miniAssumptions, false, true);
    if (res == l_True) {
      result = PSAT;
    } else if (res == l_False) {
      result = PUNSAT;
    } else {
      result = PUNKNOWN;
    }
  }

  for (auto clause : tmp) { ClauseManager::releaseClause(clause); }
  return result;
}

void MapleCOMSPSSolver::addClause(ClauseExchange* clause) {
  clausesToAdd.addClause(clause);
  setSolverInterrupt();
}

void MapleCOMSPSSolver::addLearnedClause(ClauseExchange* clause) {
  if (clause->size == 1) {
    unitsToImport.addClause(clause);
  } else {
    clausesToImport.addClause(clause);
  }
}

void MapleCOMSPSSolver::addClauses(const vector<ClauseExchange*>& clauses) {
  clausesToAdd.addClauses(clauses);
  setSolverInterrupt();
}

void MapleCOMSPSSolver::addInitialClauses(const vector<ClauseExchange*>& clauses) {
  for (size_t ind = 0; ind < clauses.size(); ind++) {
    vec<Lit> mcls;

    for (size_t i = 0; i < clauses[ind]->size; i++) {
      int lit = clauses[ind]->lits[i];
      int var = abs(lit);

      while (nVars() < var) { newVar(); }

      mcls.push(MINI_LIT(lit));
    }

    if (!static_cast<SimpSolver*>(this)->addClause(mcls)) {
      printf("c unsat when adding initial cls\n");
    }
  }
}

void MapleCOMSPSSolver::addLearnedClauses(const vector<ClauseExchange*>& clauses) {
  for (size_t i = 0; i < clauses.size(); i++) { addLearnedClause(clauses[i]); }
}

void MapleCOMSPSSolver::getLearnedClauses(vector<ClauseExchange*>& clauses) {
  clausesToExport.getClauses(clauses);
}

void MapleCOMSPSSolver::increaseClauseProduction() {
  lbdLimit++;
}

void MapleCOMSPSSolver::decreaseClauseProduction() {
  if (lbdLimit > 2) { lbdLimit--; }
}

SolvingStatistics MapleCOMSPSSolver::getStatistics() {
  SolvingStatistics stats;
  stats.conflicts = conflicts;
  stats.propagations = propagations;
  stats.restarts = starts;
  stats.decisions = decisions;
  stats.memPeak = memUsedPeak();
  return stats;
}

std::vector<int> MapleCOMSPSSolver::getModel() {
  std::vector<int> model;
  for (int i = 0; i < nVars(); i++) {
    if (this->model[i] != l_Undef) {
      int lit = this->model[i] == l_True ? i + 1 : -(i + 1);
      model.push_back(lit);
    }
  }
  return model;
}

vector<int> MapleCOMSPSSolver::getFinalAnalysis() {
  vector<int> outCls;
  for (int i = 0; i < conflict.size(); i++) { outCls.push_back(INT_LIT(conflict[i])); }
  return outCls;
}

vector<int> MapleCOMSPSSolver::getSatAssumptions() {
  vector<int> outCls;
  vec<Lit> lits;
  getAssumptions(lits);
  for (int i = 0; i < lits.size(); i++) { outCls.push_back(-(INT_LIT(lits[i]))); }
  return outCls;
}

}  // namespace painless