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

#include "../clauses/ClauseManager.h"
#include "../sharing/HordeSatSharing.h"

#include <memory>
#include "../solvers/SolverFactory.h"

namespace painless {

HordeSatSharing::HordeSatSharing(int shr_lit, int shr_sleep)
    : literalPerRound(shr_lit), roundBeforeIncrease(250000000 / shr_sleep) {}

void HordeSatSharing::doSharing(
    int idSharer, const vector<SolverInterface*>& from,
    const vector<SolverInterface*>& to) {
  static unsigned int round = 1;
  for (size_t i = 0; i < from.size(); i++) {
    int used, usedPercent, selectCount;

    int id = from[i]->id;
    if (!this->databases.count(id)) {
      this->databases[id] = std::make_unique<ClauseDatabase>();
    }

    tmp.clear();

    from[i]->getLearnedClauses(tmp);

    stats.receivedClauses += tmp.size();

    for (size_t k = 0; k < tmp.size(); k++) {
      this->databases[id]->addClause(tmp[k]);
    }

    tmp.clear();

    used =
        this->databases[id]->giveSelection(tmp, literalPerRound, &selectCount);
    usedPercent = (100 * used) / literalPerRound;

    stats.sharedClauses += tmp.size();

    if (usedPercent < 75 && !this->initPhase) {
      from[i]->increaseClauseProduction();
    } else if (usedPercent > 98) {
      from[i]->decreaseClauseProduction();
    }

    if (selectCount > 0) {
      this->initPhase = false;
    }
    if (round >= this->roundBeforeIncrease) {
      this->initPhase = false;
    }

    for (size_t j = 0; j < to.size(); j++) {
      if (from[i]->id != to[j]->id) {
        for (size_t k = 0; k < tmp.size(); k++) {
          ClauseManager::increaseClause(tmp[k], 1);
        }
        to[j]->addLearnedClauses(tmp);
      }
    }

    for (size_t k = 0; k < tmp.size(); k++) {
      ClauseManager::releaseClause(tmp[k]);
    }
  }
  round++;
}

SharingStatistics HordeSatSharing::getStatistics() {
  return stats;
}

}  // namespace painless