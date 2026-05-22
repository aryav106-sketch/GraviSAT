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

            std::string tmp;

            header >> tmp;
            header >> tmp;

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

bool Solver::checkClauses() {

    for (const auto& clause : clauseDatabase) {

        bool clauseSatisfied = false;

        for (int literal : clause) {

            int variable = std::abs(literal);

            bool value = assignment[variable];

            if (literal > 0 && value == true)
                clauseSatisfied = true;

            if (literal < 0 && value == false)
                clauseSatisfied = true;
        }

        if (!clauseSatisfied)
            return false;
    }

    return true;
}

bool Solver::dpll(int variable) {

    if (variable > variables)
        return checkClauses();

    assignment[variable] = true;

    if (dpll(variable + 1))
        return true;

    assignment[variable] = false;

    if (dpll(variable + 1))
        return true;

    assignment[variable] = -1;

    return false;
}

bool Solver::solve(const std::string& cnf) {

    parseCNF(cnf);

    return dpll(1);
}

std::string Solver::getSolution() {

    std::string result;

    for (int i = 1; i <= variables; i++) {

        result += "x";

        result += std::to_string(i);

        result += " = ";

        if (assignment[i])
            result += "TRUE";
        else
            result += "FALSE";

        result += "\n";
    }

    return result;
}
