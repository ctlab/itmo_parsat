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

namespace painless {

void SequentialWorker::main_worker_thread() {
  PSatResult res = PUNKNOWN;
  std::vector<int> model;

  while (!_stop) {
    std::unique_lock<std::mutex> ul_job(start_mutex);
    start_cv.wait(ul_job, [this] { return _stop || _task.has_value(); });
    if (_stop) { break; }
    if (!_task.has_value()) { continue; }
    auto [index, cube, assumption] = std::move(_task.value());
    _task.reset();
    ul_job.unlock();

    do {
      std::lock_guard<std::mutex> lg_int(interrupt_mutex);
      res = solver->solve(assumption, cube);
    } while (res == PUNKNOWN && !_interrupted);

    if (res == PSAT) { model = solver->getModel(); }
    join(index, nullptr, res, model);
    model.clear();
  }
}

SequentialWorker::SequentialWorker(WorkingResult* working_result, SolverInterface* solver_)
    : WorkingStrategy(working_result), solver(solver_), worker([this] { main_worker_thread(); }) {}

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

  if (worker.joinable()) { worker.join(); }
}

void SequentialWorker::awaitStop() {
  stop();
}

void SequentialWorker::solve(
    int64_t index, Mini::vec<Mini::Lit> const& assumptions, vector<int> const& cube) {
  unsetInterrupt();
  {
    std::lock_guard<std::mutex> lg(start_mutex);
    current_index = index;
    _task.emplace(index, cube, assumptions);
  }
  start_cv.notify_one();
}

void SequentialWorker::join(
    int64_t index, WorkingStrategy* winner, PSatResult res, vector<int> const& model) {
  setInterrupt();

  if (result->global_ending || current_index != index) { return; }

  if (parent == nullptr) {
    {
      std::lock_guard<std::mutex> guard(result->lock);
      result->final_result = res;
      if (res == PSAT) { result->final_model = model; }
      result->global_ending = true;
    }
    result->cv.notify_one();
  } else {
    parent->join(index, this, res, model);
  }
}

void SequentialWorker::setInterrupt() {
  solver->setSolverInterrupt();
  _interrupted = true;
}

void SequentialWorker::waitInterrupt() {
  std::lock_guard<std::mutex> lg(interrupt_mutex);
}

void SequentialWorker::unsetInterrupt() {
  _interrupted = false;
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
