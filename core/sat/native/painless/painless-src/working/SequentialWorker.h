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
#include "core/sat/native/painless/painless-src/utils/Threading.h"
#include "core/sat/native/painless/painless-src/working/WorkingStrategy.h"

#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>

namespace painless {

class SequentialWorker : public WorkingStrategy {
 public:
  SequentialWorker(WorkingResult* working_result, SolverInterface* solver_);

  ~SequentialWorker() noexcept;

  void solve(const vector<int>& cube) override;

  void join(WorkingStrategy* winner, PSatResult res, const vector<int>& model) override;

  void setInterrupt() override;

  void unsetInterrupt() override;

  void waitInterrupt() override;

  int getDivisionVariable() override;

  void setPhase(const int var, const bool phase) override;

  void stop();

  void awaitStop() override;

  void bumpVariableActivity(const int var, const int times) override;

  SolverInterface* solver;

 protected:
  void main_worker_thread();

  bool _stop = false;
  std::thread worker;
  std::mutex interrupt_lock;

  std::vector<int> actualCube;
  std::atomic_bool force;
  std::atomic_bool waitJob;

  std::mutex start_mutex;
  std::condition_variable start_cv;
};

}  // namespace painless