#include "solver.h"

#include <sstream>
#include <cstdlib>

Solver::Solver() {

    variables = 0;
    clauses = 0;
}

bool Solver::parseCNF(const std::string& cnf) {

    clauseDatabase.clear();

    std::stringstream ss(cnf);

    std::string line;

    while (std::getline(ss, line)) {

        if (line.empty())
            continue;

        if (line[0] == 'c')
            continue;

        if (line[0] == 'p') {

            std::stringstream header(line);

            std::string temp;

            header >> temp;
            header >> temp;

            header >> variables;
            header >> clauses;

            assignment.resize(variables + 1, -1);

            continue;
        }

        std::stringstream clauseStream(line);

        int literal;

        std::vector<int> clause;

        while (clauseStream >> literal) {

            if (literal == 0)
                break;

            clause.push_back(literal);
        }

        if (!clause.empty())
            clauseDatabase.push_back(clause);
    }

    return true;
}

bool Solver::propagateUnits() {

    bool changed = true;

    while (changed) {

        changed = false;

        for (const auto& clause : clauseDatabase) {

            int unassignedCount = 0;

            int lastLiteral = 0;

            bool clauseSatisfied = false;

            for (int literal : clause) {

                int variable = std::abs(literal);

                int value = assignment[variable];

                if (value == -1) {

                    unassignedCount++;
                    lastLiteral = literal;
                }
                else {

                    if ((literal > 0 && value == 1) ||
                        (literal < 0 && value == 0)) {

                        clauseSatisfied = true;
                        break;
                    }
                }
            }

            if (clauseSatisfied)
                continue;

            if (unassignedCount == 0)
                return false;

            if (unassignedCount == 1) {

                int variable = std::abs(lastLiteral);

                assignment[variable] =
                        (lastLiteral > 0) ? 1 : 0;

                changed = true;
            }
        }
    }

    return true;
}

bool Solver::hasConflict() {

    for (const auto& clause : clauseDatabase) {

        bool satisfied = false;

        bool undecided = false;

        for (int literal : clause) {

            int variable = std::abs(literal);

            int value = assignment[variable];

            if (value == -1)
                undecided = true;

            if ((literal > 0 && value == 1) ||
                (literal < 0 && value == 0)) {

                satisfied = true;
            }
        }

        if (!satisfied && !undecided)
            return true;
    }

    return false;
}

bool Solver::allClausesSatisfied() {

    for (const auto& clause : clauseDatabase) {

        bool satisfied = false;

        for (int literal : clause) {

            int variable = std::abs(literal);

            int value = assignment[variable];

            if ((literal > 0 && value == 1) ||
                (literal < 0 && value == 0)) {

                satisfied = true;
                break;
            }
        }

        if (!satisfied)
            return false;
    }

    return true;
}

bool Solver::dpll() {

    if (!propagateUnits())
        return false;

    if (hasConflict())
        return false;

    if (allClausesSatisfied())
        return true;

    int variable = -1;

    for (int i = 1; i <= variables; i++) {

        if (assignment[i] == -1) {

            variable = i;
            break;
        }
    }

    if (variable == -1)
        return false;

    assignment[variable] = 1;

    if (dpll())
        return true;

    assignment[variable] = 0;

    if (dpll())
        return true;

    assignment[variable] = -1;

    return false;
}

bool Solver::solve(const std::string& cnf) {

    parseCNF(cnf);

    return dpll();
}

std::string Solver::getSolution() {

    std::string output;

    for (int i = 1; i <= variables; i++) {

        output += "x";

        output += std::to_string(i);

        output += " = ";

        if (assignment[i] == 1)
            output += "TRUE";
        else if (assignment[i] == 0)
            output += "FALSE";
        else
            output += "UNASSIGNED";

        output += "\n";
    }

    return output;
}
