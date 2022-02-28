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
    ul_job.unlock();

    {
      std::lock_guard<std::mutex> lg_int(interrupt_lock);
      do {
        res = solver->solve(actualCube);
      } while (!force && res == PUNKNOWN);
    }

    if (res == PSAT) {
      model = solver->getModel();
    }

    join(nullptr, res, model);
    model.clear();
    waitJob = true;
  }
}

SequentialWorker::SequentialWorker(WorkingResult* working_result, SolverInterface* solver_)
    : WorkingStrategy(working_result) {
  solver = solver_;
  force = false;
  waitJob = true;
  worker = std::thread([this] { main_worker_thread(); });
}

SequentialWorker::~SequentialWorker() noexcept {
  stop();
}

void SequentialWorker::stop() {
  force = true;
  solver->setSolverInterrupt();
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

void SequentialWorker::solve(const vector<int>& cube) {
  actualCube = cube;
  unsetInterrupt();
  {
    std::lock_guard<std::mutex> lg(start_mutex);
    waitJob = false;
  }
  start_cv.notify_one();
}

void SequentialWorker::join(WorkingStrategy* winner, PSatResult res, const vector<int>& model) {
  force = true;

  if (result->globalEnding)
    return;

  if (parent == NULL) {
    result->globalEnding = true;
    result->finalResult = res;

    if (res == PSAT) {
      result->finalModel = model;
    }
  } else {
    parent->join(this, res, model);
  }
}

void SequentialWorker::waitInterrupt() {
  std::lock_guard<std::mutex> lg(interrupt_lock);
}

void SequentialWorker::setInterrupt() {
  force = true;
  solver->setSolverInterrupt();
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
