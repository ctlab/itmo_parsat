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

#include "../working/SequentialWorker.h"
#include "util/stream.h"

#include <unistd.h>

namespace painless {

void SequentialWorker::main_worker_thread() {
  PSatResult res = PUNKNOWN;
  std::vector<int> model;

  while (!_stop) {
    std::unique_lock<std::mutex> ul_job(start_mutex);
    start_cv.wait(ul_job, [this] { return _stop || !waitJob; });
    if (_stop) {
      break;
    }
    if (waitJob) {
      continue;
    }
    auto local_assumptions = actualAssumptions;
    auto local_cube = actualCube;
    auto local_index = actual_index;
    ul_job.unlock();

    {
      std::lock_guard<std::mutex> lg_int(interrupt_mutex);
      res = solver->solve(local_assumptions, local_cube);
    }

    if (current_index == local_index) {
      if (res == PSAT) {
        model = solver->getModel();
      }
      join(local_index, nullptr, res, model);
      model.clear();
    }

    waitJob = true;
  }
}

SequentialWorker::SequentialWorker(
    WorkingResult* working_result, SolverInterface* solver_)
    : WorkingStrategy(working_result)
    , solver(solver_)
    , worker([this] { main_worker_thread(); }) {}

SequentialWorker::~SequentialWorker() noexcept {
  stop();
}

void SequentialWorker::stop() {
  setInterrupt();
  {
    std::lock_guard<std::mutex> lg(start_mutex);
    _stop = true;
  }
  start_cv.notify_all();

  if (worker.joinable()) {
    worker.join();
  }
}

void SequentialWorker::awaitStop() {
  stop();
}

void SequentialWorker::solve(
    int64_t index, Mini::vec<Mini::Lit> const& assumptions,
    vector<int> const& cube) {
  {
    std::lock_guard<std::mutex> lg(start_mutex);
    actual_index = current_index = index;
    actualCube = cube;
    actualAssumptions = assumptions;
    waitJob = false;
  }
  start_cv.notify_one();
}

void SequentialWorker::join(
    int64_t index, WorkingStrategy* winner, PSatResult res,
    const vector<int>& model) {
  setInterrupt();

  if (result->globalEnding || current_index != index) {
    return;
  }

  if (parent == NULL) {
    result->globalEnding = true;
    result->finalResult = res;

    if (res == PSAT) {
      result->finalModel = model;
    }
  } else {
    parent->join(index, this, res, model);
  }
}

void SequentialWorker::setInterrupt() {
  force = true;
  solver->setSolverInterrupt();
}

void SequentialWorker::waitInterrupt() {
  std::lock_guard<std::mutex> lg(interrupt_mutex);
}

void SequentialWorker::unsetInterrupt() {
  force = false;
  solver->unsetSolverInterrupt();
}

int SequentialWorker::getDivisionVariable() {
  return solver->getDivisionVariable();
}

void SequentialWorker::setPhase(const int var, const bool phase) {
  solver->setPhase(var, phase);
}

void SequentialWorker::bumpVariableActivity(const int var, const int times) {
  solver->bumpVariableActivity(var, times);
}

}  // namespace painless
