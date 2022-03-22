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

#include "../clauses/ClauseDatabase.h"
#include "../sharing/SharingStrategy.h"
#include "../solvers/SolverInterface.h"

#include <unordered_map>
#include <vector>

namespace painless {

class HordeSatSharing : public SharingStrategy {
 public:
  explicit HordeSatSharing(int shr_lit, int shr_sleep, WorkingResult* result);

  ~HordeSatSharing() = default;

  void doSharing(
      int idSharer, const std::vector<SolverInterface*>& from,
      const std::vector<SolverInterface*>& to);

  SharingStatistics getStatistics();

 protected:
  int literalPerRound;
  bool initPhase = true;
  int roundBeforeIncrease;

  std::unordered_map<int, std::unique_ptr<ClauseDatabase>> databases;
  SharingStatistics stats;
  std::vector<ClauseExchange*> tmp;
};

}  // namespace painless