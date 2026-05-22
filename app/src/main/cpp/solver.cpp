#include "solver.h"
#include "dimacs_parser.h"

Solver::Solver() {

    variables = 0;
    clauses = 0;
}

bool Solver::parseCNF(const std::string& cnf) {

    return DimacsParser().parse(cnf);
}

bool Solver::dpll() {

    return true;
}

bool Solver::solve(const std::string& cnf) {

    bool valid = parseCNF(cnf);

    if (!valid) {
        return false;
    }

    return dpll();
}
