#include "dimacs_parser.h"
#include <fstream>
#include <sstream>

CNFFormula parseDIMACS(const std::string& filename) {

    CNFFormula formula;
    formula.variables = 0;

    std::ifstream file(filename);

    std::string line;

    while (std::getline(file, line)) {

        if (line.empty()) continue;

        if (line[0] == 'c')
            continue;

        if (line[0] == 'p') {

            std::stringstream ss(line);

            std::string tmp;

            int clauses;

            ss >> tmp >> tmp >> formula.variables >> clauses;

            continue;
        }

        std::stringstream ss(line);

        Clause clause;

        int literal;

        while (ss >> literal) {

            if (literal == 0)
                break;

            clause.literals.push_back(literal);
        }

        formula.clauses.push_back(clause);
    }

    return formula;
}
