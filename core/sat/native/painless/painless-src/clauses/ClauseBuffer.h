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

#include "../clauses/ClauseExchange.h"

#include <atomic>
#include <memory>
#include <stdio.h>
#include <vector>

namespace painless {

class ClauseBuffer {
 public:
  ClauseBuffer();

  ~ClauseBuffer();

  void addClause(ClauseExchange* clause);

  void addClauses(const std::vector<ClauseExchange*>& clauses);

  bool getClause(ClauseExchange** clause);

  void getClauses(std::vector<ClauseExchange*>& clauses);

  int size();

 protected:
  typedef struct ListElement {
    ClauseExchange* clause;

    std::atomic<ListElement*> next;

    ListElement(ClauseExchange* cls) {
      next = NULL;
      clause = cls;
    }

    ~ListElement() {}
  } ListElement;

  typedef struct ListRoot {
    std::atomic_int size;

    std::atomic<ListElement*> head;
    std::atomic<ListElement*> tail;
  } ListRoot;

  ListRoot buffer;
};

}  // namespace painless