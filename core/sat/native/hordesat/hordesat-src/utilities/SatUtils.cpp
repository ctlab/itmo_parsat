// Copyright (c) 2015 Tomas Balyo, Karlsruhe Institute of Technology
/*
 * SatUtils.cpp
 *
 *  Created on: Mar 9, 2015
 *      Author: balyo
 */

#include "SatUtils.h"
#include <ctype.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>

#include "util/options.h"

bool loadFormulaToSolvers(vector<PortfolioSolverInterface*>& solvers, const char* filename) {
  FILE* f = fopen(filename, "r");
  if (f == NULL) {
    return false;
  }
  int c = 0;
  bool neg = false;
  vector<vector<int>> clauses;
  vector<int> cls;
  while (c != EOF) {
    c = fgetc(f);

    // comment or problem definition line
    if (c == 'c' || c == 'p') {
      // skip this line
      while (c != '\n') {
        c = fgetc(f);
      }
      continue;
    }
    // whitespace
    if (isspace(c)) {
      continue;
    }
    // negative
    if (c == '-') {
      neg = true;
      continue;
    }

    // number
    if (isdigit(c)) {
      int num = c - '0';
      c = fgetc(f);
      while (isdigit(c)) {
        num = num * 10 + (c - '0');
        c = fgetc(f);
      }
      if (neg) {
        num *= -1;
      }
      neg = false;

      if (num != 0) {
        cls.push_back(num);
      } else {
        clauses.push_back(vector<int>(cls));
        cls.clear();
      }
    }
  }
  fclose(f);

  loadClausesToSolvers(solvers, clauses);
  return true;
}

void loadClausesToSolvers(
    vector<PortfolioSolverInterface*>& solvers, vector<vector<int>> const& clauses) {
  std::for_each(IPS_EXEC_POLICY, solvers.begin(), solvers.end(), [&clauses](auto& solver) {
    solver->addInitialClauses(clauses);
  });
}
