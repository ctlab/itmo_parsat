// -----------------------------------------------------------------------------
// Copyright (C) 2017  Ludovic LE FRIOUX
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

#include "painless.h"

#include "utils/Logger.h"
#include "utils/Parameters.h"
#include "utils/System.h"
#include "utils/SatUtils.h"

#include "solvers/SolverFactory.h"

#include "clauses/ClauseManager.h"

#include "sharing/HordeSatSharing.h"
#include "sharing/Sharer.h"

#include "working/SequentialWorker.h"
#include "working/Portfolio.h"

#include <unistd.h>

using namespace std;
using namespace painless;

// -------------------------------------------
// Declaration of global variables
// -------------------------------------------
Sharer** sharers = NULL;

int nSharers = 0;

WorkingStrategy* working = NULL;

// -------------------------------------------
// Main of the framework
// -------------------------------------------
int main(int argc, char** argv) {
  Parameters::init(argc, argv);

  if (Parameters::getFilename() == NULL || Parameters::getBoolParam("h")) {
    cout << "USAGE: " << argv[0] << " [options] input.cnf" << endl;
    cout << "Options:" << endl;
    cout << "\t-c=<INT>\t\t number of cpus, default is 24" << endl;
    cout << "\t-max-memory=<INT>\t memory limit in GB, default is 200" << endl;
    cout << "\t-t=<INT>\t\t timeout in seconds, default is no limit" << endl;
    cout << "\t-lbd-limit=<INT>\t LBD limit of exported clauses, default is"
            " 2"
         << endl;
    cout << "\t-shr-sleep=<INT>\t time in useconds a sharer sleep each "
            "round, default is 500000 (0.5s)"
         << endl;
    cout << "\t-shr-lit=<INT>\t\t number of literals shared per round, "
            "default is 1500"
         << endl;
    cout << "\t-v=<INT>\t\t verbosity level, default is 0" << endl;
    return 0;
  }

  // Parameters::printParams();

  int cpus = Parameters::getIntParam("c", 30);
  setVerbosityLevel(Parameters::getIntParam("v", 0));

  // Create and init solvers
  vector<SolverInterface*> solvers;
  vector<SolverInterface*> solvers_VSIDS;
  vector<SolverInterface*> solvers_LRB;

  SolverFactory::createMapleCOMSPSSolvers(2, 0, cpus - 2, solvers);
  solvers.push_back(SolverFactory::createReducerSolver(
      SolverFactory::createMapleCOMSPSSolver(2)));
  solvers.push_back(SolverFactory::createReducerSolver(
      SolverFactory::createMapleCOMSPSSolver(2)));
  int nSolvers = solvers.size();

  SolverFactory::nativeDiversification(solvers);

  for (int id = 0; id < nSolvers; id++) {
    if (id % 2) {
      solvers_LRB.push_back(solvers[id]);
    } else {
      solvers_VSIDS.push_back(solvers[id]);
    }
  }

  SolverFactory::sparseRandomDiversification(solvers_LRB);
  SolverFactory::sparseRandomDiversification(solvers_VSIDS);

  // Init Sharing
  // 15 CDCL, 1 Reducer producers by Sharer
  vector<SolverInterface*> prod1;
  vector<SolverInterface*> prod2;
  vector<SolverInterface*> reducerCons1;
  vector<SolverInterface*> reducerCons2;
  vector<SolverInterface*> cons1;
  vector<SolverInterface*> cons2;
  vector<SolverInterface*> consCDCL;

  WorkingResult result;

  working = new Portfolio(&result);

  prod1.insert(prod1.end(), solvers.begin(), solvers.begin() + (cpus / 2 - 1));
  prod1.push_back(solvers[solvers.size() - 2]);
  prod2.insert(
      prod2.end(), solvers.begin() + (cpus / 2 - 1), solvers.end() - 2);
  prod2.push_back(solvers[solvers.size() - 1]);
  // 30 CDCL, 1 Reducer consumers by Sharer
  cons1.insert(cons1.end(), solvers.begin(), solvers.end() - 1);
  cons2.insert(cons2.end(), solvers.begin(), solvers.end() - 2);
  cons2.push_back(solvers[solvers.size() - 1]);
  nSharers = 2;
  sharers = new Sharer*[nSharers];
  sharers[0] = new Sharer(
      500000, 1, new HordeSatSharing(1500, 500000, &result), prod1, cons1);
  sharers[1] = new Sharer(
      500000, 2, new HordeSatSharing(1500, 500000, &result), prod2, cons2);

  // Init working
  for (size_t i = 0; i < nSolvers; i++) {
    working->addSlave(new SequentialWorker(&result, solvers[i]));
  }

  char const* cf = Parameters::getFilename();
  for (auto solver : solvers) {
    solver->loadFormula(cf);
  }

  vector<int> cube;
  working->solve(0, {}, cube);

  int timeout = Parameters::getIntParam("t", -1);

  while (!working->result->globalEnding) {
    sleep(1);

    if (timeout > 0 && getRelativeTime() >= timeout) {
      working->result->globalEnding = true;
      working->setInterrupt();
    }
  }

  if (working->result->finalResult == PSAT) {
    cout << "s SATISFIABLE" << endl;
    if (!Parameters::getBoolParam("no-model")) {
      printModel(working->result->finalModel);
    }
  } else if (working->result->finalResult == PUNSAT) {
    cout << "s UNSATISFIABLE" << endl;
  } else {
    cout << "s UNKNOWN" << endl;
  }

  return 0;
}
