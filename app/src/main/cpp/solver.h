#ifndef GRAVISAT_SOLVER_H
#define GRAVISAT_SOLVER_H

#include <vector>
#include <string>

struct Clause {

    std::vector<int> literals;

    int watch1;
    int watch2;
};

class Solver {

public:

    Solver();

    bool solve(const std::string& cnf);

    std::string getSolution();

private:

    bool parseCNF(const std::string& cnf);

    bool dpll();

    bool propagate();

    bool hasConflict();

    bool allSatisfied();

    int chooseVariable();

private:

    int variables;
    int clauses;

    std::vector<Clause> clauseDatabase;

    std::vector<int> assignment;
};

#endif
