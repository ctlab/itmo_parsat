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

#include <algorithm>

#include "../working/Portfolio.h"
#include "../working/SequentialWorker.h"

#include "util/options.h"

using namespace std;

namespace painless {

Portfolio::Portfolio(WorkingResult* working_result) : WorkingStrategy(working_result) {}

Portfolio::~Portfolio() {
  std::for_each(IPS_EXEC_POLICY, slaves.begin(), slaves.end(), [&](auto* slave) {
    slave->setInterrupt();
    delete slave;
  });
}

void Portfolio::solve(int64_t index, Mini::vec<Mini::Lit> const& assumptions, vector<int> const& cube) {
  current_index = index;
  strategyEnding = false;
  std::for_each(
      IPS_EXEC_POLICY, slaves.begin(), slaves.end(), [&](auto& slave) { slave->solve(index, assumptions, cube); });
}

void Portfolio::join(int64_t index, WorkingStrategy* strat, PSatResult res, Mini::vec<Mini::lbool> const& model) {
  std::lock_guard<std::mutex> lg(join_mutex);
  if (res == PUNKNOWN || strategyEnding || result->global_ending || index != current_index) {
    return;
  }

  strategyEnding = true;
  setInterrupt();

  if (parent == nullptr) {
    {
      std::lock_guard<std::mutex> guard(result->lock);
      result->final_result = res;
      if (res == PSAT) {
        result->final_model = model;
      }
      result->global_ending = true;
    }
    result->cv.notify_one();
  } else {
    parent->join(index, this, res, model);
  }
}

void Portfolio::setInterrupt() {
  std::for_each(IPS_EXEC_POLICY, slaves.begin(), slaves.end(), [](auto& slave) { slave->setInterrupt(); });
}

void Portfolio::waitInterrupt() {
  std::for_each(IPS_EXEC_POLICY, slaves.begin(), slaves.end(), [](auto& slave) { slave->waitInterrupt(); });
}

void Portfolio::unsetInterrupt() {
  std::for_each(IPS_EXEC_POLICY, slaves.begin(), slaves.end(), [](auto& slave) { slave->unsetInterrupt(); });
}

void Portfolio::awaitStop() {
  std::for_each(IPS_EXEC_POLICY, slaves.begin(), slaves.end(), [](auto& slave) { slave->awaitStop(); });
}

int Portfolio::getDivisionVariable() { return 0; }

void Portfolio::setPhase(int var, bool value) {}

void Portfolio::bumpVariableActivity(int var, int times) {}

}  // namespace painless
