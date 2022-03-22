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

#include <atomic>
#include <stdlib.h>

#include "../clauses/ClauseExchange.h"

namespace painless {

class ClauseManager {
 public:
  ClauseManager() = default;

  static ClauseExchange* allocClause(int size) {
    ClauseExchange* ptr;
    ptr = (ClauseExchange*) malloc(sizeof(ClauseExchange) + sizeof(int) * size);
    ptr->size = size;
    ptr->nbRefs = 1;
    return ptr;
  }

  static void increaseClause(ClauseExchange* cls, int refs = 1) {
    cls->nbRefs += refs;
  }

  static void releaseClause(ClauseExchange* cls) {
    int oldValue = cls->nbRefs.fetch_sub(1);

    if (oldValue - 1 <= 0) {
      free(cls);
    }
  }
};

}  // namespace painless