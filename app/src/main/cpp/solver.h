#ifndef GRAVISAT_SOLVER_H
#define GRAVISAT_SOLVER_H

#include <vector>
#include <string>

class Solver {

public:

    Solver();

    bool solve(const std::string& cnf);

    std::string getSolution();

private:

    bool parseCNF(const std::string& cnf);

    bool dpll(int variable);

    bool checkClauses();

private:

    int variables;
    int clauses;

    std::vector<std::vector<int>> clauseDatabase;

    std::vector<int> assignment;
};

#endif
