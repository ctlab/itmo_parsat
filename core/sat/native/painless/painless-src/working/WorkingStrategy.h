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

#include "core/sat/native/painless/painless-src/solvers/SolverInterface.h"

#include <vector>
#include <memory>
#include <mutex>
#include <condition_variable>

#define IPS_PRINTF(...)         \
  fprintf(stderr, __VA_ARGS__); \
  fflush(stderr)

namespace painless {

struct WorkingResult {
  std::mutex lock;
  std::condition_variable cv;
  bool global_ending = false;
  PSatResult final_result = PUNKNOWN;
  std::vector<int> final_model;
};

extern std::mutex m_pf;

class WorkingStrategy {
 public:
  explicit WorkingStrategy(WorkingResult* working_result);

  virtual ~WorkingStrategy() = default;

  virtual void solve(int64_t index, Mini::vec<Mini::Lit> const& assumptions, const vector<int>& cube) = 0;

  virtual void join(int64_t index, WorkingStrategy* winner, PSatResult res, const vector<int>& model) = 0;

  virtual int getDivisionVariable() = 0;

  virtual void setPhase(const int var, const bool value) = 0;

  virtual void bumpVariableActivity(const int var, const int times) = 0;

  virtual void setInterrupt() = 0;

  virtual void unsetInterrupt() = 0;

  virtual void waitInterrupt() = 0;

  virtual void addSlave(WorkingStrategy* slave);

  virtual void awaitStop() = 0;

 public:
  WorkingResult* result = nullptr;

 protected:
  WorkingStrategy* parent = nullptr;
  std::vector<WorkingStrategy*> slaves;
};

}  // namespace painless
