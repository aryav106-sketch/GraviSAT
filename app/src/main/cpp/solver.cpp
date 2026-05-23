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

        Clause clause;

        while (clauseStream >> literal) {

            if (literal == 0)
                break;

            clause.literals.push_back(literal);
        }

        if (!clause.literals.empty()) {

            clause.watch1 = 0;

            if (clause.literals.size() > 1)
                clause.watch2 = 1;
            else
                clause.watch2 = 0;

            clauseDatabase.push_back(clause);
        }
    }

    return true;
}

bool Solver::propagate() {

    bool changed = true;

    while (changed) {

        changed = false;

        for (auto& clause : clauseDatabase) {

            bool satisfied = false;

            int unassigned = 0;

            int lastLiteral = 0;

            for (int literal : clause.literals) {

                int variable = std::abs(literal);

                int value = assignment[variable];

                if (value == -1) {

                    unassigned++;
                    lastLiteral = literal;
                }
                else {

                    if ((literal > 0 && value == 1) ||
                        (literal < 0 && value == 0)) {

                        satisfied = true;
                        break;
                    }
                }
            }

            if (satisfied)
                continue;

            if (unassigned == 0)
                return false;

            if (unassigned == 1) {

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

        for (int literal : clause.literals) {

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

bool Solver::allSatisfied() {

    for (const auto& clause : clauseDatabase) {

        bool satisfied = false;

        for (int literal : clause.literals) {

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

int Solver::chooseVariable() {

    for (int i = 1; i <= variables; i++) {

        if (assignment[i] == -1)
            return i;
    }

    return -1;
}

bool Solver::dpll() {

    if (!propagate())
        return false;

    if (hasConflict())
        return false;

    if (allSatisfied())
        return true;

    int variable = chooseVariable();

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
