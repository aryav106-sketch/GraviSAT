#include "solver.h"

Solver::Solver() {
    variables = 0;
    clauses = 0;
}

bool Solver::solve(const std::string& cnf) {
    return parseCNF(cnf) && dpll();
}

bool Solver::parseCNF(const std::string& cnf) {
    return true;
}

bool Solver::dpll() {
    return true;
}
