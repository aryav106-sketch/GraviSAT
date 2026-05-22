#pragma once

#include "dimacs_parser.h"
#include <vector>

class Solver {

public:

    CNFFormula formula;

    explicit Solver(const CNFFormula& f);

    bool solve();

private:

    std::vector<int> assignment;

    bool dpll();

    bool isSatisfied();

    bool hasConflict();

    int chooseVariable();
};
