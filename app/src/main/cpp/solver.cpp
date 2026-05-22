#include "solver.h"

Solver::Solver()
        : variables(0),
          clauses(0) {
}

bool Solver::parseCNF(const std::string& cnf) {

    if (cnf.empty()) {
        return false;
    }

    return true;
}

bool Solver::dpll() {

    return true;
}

bool Solver::solve(const std::string& cnf) {

    bool parsed = parseCNF(cnf);

    if (!parsed) {
        return false;
    }

    return dpll();
}
