// Copyright (c) 2015 Tomas Balyo, Karlsruhe Institute of Technology
/*
 * satUtils.h
 *
 *  Created on: Dec 4, 2014
 *      Author: balyo
 */

#ifndef SATUTILS_H_
#define SATUTILS_H_

#include "../solvers/PortfolioSolverInterface.h"

bool loadFormulaToSolvers(vector<PortfolioSolverInterface*>& solvers, const char* filename);

void loadClausesToSolvers(
    vector<PortfolioSolverInterface*>& solvers, vector<vector<int>> const& clauses);

#endif /* SATUTILS_H_ */
