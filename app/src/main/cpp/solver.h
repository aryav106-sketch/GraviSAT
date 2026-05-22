#ifndef GRAVISAT_SOLVER_H
#define GRAVISAT_SOLVER_H

#include <string>

class Solver {

public:

    Solver();

    bool solve(const std::string& cnf);

private:

    bool parseCNF(const std::string& cnf);

    bool dpll(const std::string& cnf);

private:

    int variables;
    int clauses;
};

#endif
