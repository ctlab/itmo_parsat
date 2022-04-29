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

#include "ClauseBuffer.h"
#include "ClauseManager.h"

#include <iostream>

namespace painless {

ClauseBuffer::ClauseBuffer() {
  ListElement* node = new ListElement(nullptr);
  buffer.head = buffer.tail = node;
  buffer.size = 0;
}

ClauseBuffer::~ClauseBuffer() {
  if (buffer.head.load() != buffer.tail.load()) {
    ListElement *node = buffer.head.load()->next, *next;
    while (node != nullptr) {
      next = node->next;
      ClauseManager::releaseClause(node->clause);
      delete node;
      node = next;
    }
  }
  delete buffer.head;
}

void ClauseBuffer::addClause(ClauseExchange* clause) {
  ListElement *tail, *next;
  ListElement* node = new ListElement(clause);

  while (true) {
    tail = buffer.tail;
    next = tail->next;

    if (tail == buffer.tail) {
      if (next == NULL) {
        if (tail->next.compare_exchange_strong(next, node)) {
          buffer.size++;
          break;
        }
      } else {
        buffer.tail.compare_exchange_strong(tail, next);
      }
    }
  }

  buffer.tail.compare_exchange_strong(tail, node);
}

void ClauseBuffer::addClauses(const std::vector<ClauseExchange*>& clauses) {
  for (int i = 0; i < clauses.size(); i++) {
    addClause(clauses[i]);
  }
}

bool ClauseBuffer::getClause(ClauseExchange** clause) {
  ListElement *head, *tail, *next;

  while (true) {
    head = buffer.head;
    tail = buffer.tail;
    next = head->next;

    if (head == buffer.head) {
      if (head == tail) {
        if (next == nullptr)
          return false;

        buffer.tail.compare_exchange_strong(tail, next);
      } else {
        *clause = next->clause;

        if (buffer.head.compare_exchange_strong(head, next)) {
          break;
        }
      }
    }
  }

  delete head;
  buffer.size--;
  return true;
}

void ClauseBuffer::getClauses(std::vector<ClauseExchange*>& clauses) {
  ClauseExchange* cls;

  int nClauses = size();
  int nClausesGet = 0;

  while (getClause(&cls) && nClausesGet < nClauses) {
    clauses.push_back(cls);
    nClausesGet++;
  }
}

int ClauseBuffer::size() { return buffer.size; }

}  // namespace painless