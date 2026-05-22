#ifndef GRAVISAT_SOLVER_H
#define GRAVISAT_SOLVER_H

#include <string>
#include <vector>

class Solver {

public:

    Solver();

    bool solve(const std::string& cnf);

private:

    bool parseCNF(const std::string& cnf);

    bool dpll();

private:

    int variables;
    int clauses;
};
#endif
