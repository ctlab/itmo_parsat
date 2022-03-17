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
#include "core/sat/native/painless/painless-src/working/WorkingStrategy.h"

#include <vector>

namespace painless {

struct SharingStatistics {
  unsigned long sharedClauses = 0;
  unsigned long receivedClauses = 0;
};

class SharingStrategy {
 public:
  explicit SharingStrategy(WorkingResult* result) : result(result) {}

  virtual ~SharingStrategy() = default;

  virtual void doSharing(
      int idSharer, const vector<SolverInterface*>& producers,
      const vector<SolverInterface*>& consumers) = 0;

  virtual SharingStatistics getStatistics() = 0;

 public:
  WorkingResult* result = nullptr;
};

}  // namespace painless