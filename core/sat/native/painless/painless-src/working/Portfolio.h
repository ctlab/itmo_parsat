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

#include "core/sat/native/painless/painless-src/working/WorkingStrategy.h"

namespace painless {

class Portfolio : public WorkingStrategy {
 public:
  Portfolio(WorkingResult* working_result);

  ~Portfolio() noexcept;

  void solve(const vector<int>& cube) override;

  void join(WorkingStrategy* strat, PSatResult res, const vector<int>& model) override;

  void setInterrupt() override;

  void unsetInterrupt() override;

  void waitInterrupt() override;

  int getDivisionVariable() override;

  void setPhase(int var, bool value) override;

  void awaitStop() override;

  void bumpVariableActivity(int var, int times) override;

 protected:
  std::atomic<bool> strategyEnding;
};

}  // namespace painless
