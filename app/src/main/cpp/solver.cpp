#include "solver.h"

Solver::Solver(const CNFFormula& f) : formula(f) {
    assignment.resize(formula.variables + 1, -1);
}

bool Solver::solve() {
    return dpll();
}

bool Solver::isSatisfied() {

    for (const auto& clause : formula.clauses) {

        bool satisfied = false;

        for (int lit : clause.literals) {

            int var = abs(lit);

            int val = assignment[var];

            if (val == -1)
                continue;

            if ((lit > 0 && val == 1) ||
                (lit < 0 && val == 0)) {

                satisfied = true;
                break;
            }
        }

        if (!satisfied)
            return false;
    }

    return true;
}

bool Solver::hasConflict() {

    for (const auto& clause : formula.clauses) {

        bool possible = false;

        for (int lit : clause.literals) {

            int var = abs(lit);

            int val = assignment[var];

            if (val == -1) {
                possible = true;
                break;
            }

            if ((lit > 0 && val == 1) ||
                (lit < 0 && val == 0)) {

                possible = true;
                break;
            }
        }

        if (!possible)
            return true;
    }

    return false;
}

int Solver::chooseVariable() {

    for (int i = 1; i <= formula.variables; i++) {

        if (assignment[i] == -1)
            return i;
    }

    return -1;
}

bool Solver::dpll() {

    if (hasConflict())
        return false;

    if (isSatisfied())
        return true;

    int var = chooseVariable();

    if (var == -1)
        return false;

    assignment[var] = 1;

    if (dpll())
        return true;

    assignment[var] = 0;

    if (dpll())
        return true;

    assignment[var] = -1;

    return false;
}
