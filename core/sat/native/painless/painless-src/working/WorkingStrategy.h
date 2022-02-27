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

#include "../solvers/SolverInterface.h"

#include <vector>
#include <memory>

using namespace std;

struct WorkingResult {
  atomic<bool> globalEnding{false};
  PSatResult finalResult = PUNKNOWN;
  vector<int> finalModel;
};

using RWorkingResult = std::shared_ptr<WorkingResult>;

class WorkingStrategy {
 public:
  WorkingStrategy() {
    parent = NULL;
    result = std::make_shared<WorkingResult>();
  }

  virtual ~WorkingStrategy() = default;

  virtual void solve(const vector<int>& cube) = 0;

  virtual void join(WorkingStrategy* winner, PSatResult res, const vector<int>& model) = 0;

  virtual int getDivisionVariable() = 0;

  virtual void setPhase(const int var, const bool value) = 0;

  virtual void bumpVariableActivity(const int var, const int times) = 0;

  virtual void setInterrupt() = 0;

  virtual void unsetInterrupt() = 0;

  virtual void waitInterrupt() = 0;

  virtual void addSlave(WorkingStrategy* slave) {
    slaves.push_back(slave);
    slave->parent = this;
    slave->result = result;
  }

 public:
  RWorkingResult result;

 protected:
  WorkingStrategy* parent;
  vector<WorkingStrategy*> slaves;
};
