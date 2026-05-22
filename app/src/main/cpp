#pragma once

#include <vector>
#include <string>

struct Clause {
    std::vector<int> literals;
};

struct CNFFormula {
    int variables;
    std::vector<Clause> clauses;
};

CNFFormula parseDIMACS(const std::string& filename);
