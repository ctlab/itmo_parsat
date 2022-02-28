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

#include "../solvers/MapleCOMSPSSolver.h"
#include "../solvers/SolverFactory.h"
#include "../solvers/Reducer.h"
#include "../utils/System.h"

void SolverFactory::sparseRandomDiversification(const vector<SolverInterface*>& solvers) {
  if (solvers.size() == 0)
    return;

  int vars = solvers[0]->getVariablesCount();

  // The first solver of the group (1 LRB/1 VSIDS) keeps polarity = false for all vars
  for (int sid = 1; sid < solvers.size(); sid++) {
    srand(sid);
    for (int var = 1; var <= vars; var++) {
      if (rand() % solvers.size() == 0) {
        solvers[sid]->setPhase(var, rand() % 2 == 1);
      }
    }
  }
}

void SolverFactory::nativeDiversification(const vector<SolverInterface*>& solvers) {
  for (int sid = 0; sid < solvers.size(); sid++) {
    solvers[sid]->diversify(sid);
  }
}

SolverInterface* SolverFactory::createMapleCOMSPSSolver(int lbd_limit) {
  return new MapleCOMSPSSolver(currentIdSolver.fetch_add(1), lbd_limit);
}

SolverInterface* SolverFactory::createReducerSolver(SolverInterface* _solver) {
  return new Reducer(currentIdSolver.fetch_add(1), _solver);
}

void SolverFactory::createMapleCOMSPSSolvers(
    int lbd_limit, int max_memory, int maxSolvers, vector<SolverInterface*>& solvers) {
  solvers.push_back(createMapleCOMSPSSolver(lbd_limit));
  double memoryUsed = getMemoryUsed();

  if (max_memory && maxSolvers > max_memory) {
    maxSolvers = max_memory;
  }

  for (int i = 1; i < maxSolvers; i++) {
    solvers.push_back(createMapleCOMSPSSolver(lbd_limit));
  }
}

SolverInterface* SolverFactory::cloneSolver(SolverInterface* other) {
  SolverInterface* solver;
  int id = currentIdSolver.fetch_add(1);
  auto&& o = (MapleCOMSPSSolver&) *other;
  return new MapleCOMSPSSolver(id, o.lbdLimit, (MapleCOMSPSSolver&) *other);
}
