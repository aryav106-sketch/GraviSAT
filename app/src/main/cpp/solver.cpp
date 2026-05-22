#include "solver.h"
#include <sstream>
#include <cstdlib>

Solver::Solver() {
    variables = 0;
    clauses = 0;
}

bool Solver::parseCNF(const std::string& cnf) {

    std::stringstream ss(cnf);
    std::string token;

    while (ss >> token) {

        if (token == "p") {
            std::string format;
            ss >> format;
            ss >> variables;
            ss >> clauses;
        }
    }

    return true;
}

bool Solver::dpll(const std::string& cnf) {

    // VERY SIMPLE TEST SOLVER

    // Detect contradiction:
    // "1 0" and "-1 0"

    bool positive = false;
    bool negative = false;

    std::stringstream ss(cnf);
    int lit;

    while (ss >> lit) {

        if (lit == 1)
            positive = true;

        if (lit == -1)
            negative = true;
    }

    if (positive && negative)
        return false;

    return true;
}

bool Solver::solve(const std::string& cnf) {

    parseCNF(cnf);

    return dpll(cnf);
}
