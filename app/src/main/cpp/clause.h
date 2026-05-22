#ifndef GRAVISAT_CLAUSE_H
#define GRAVISAT_CLAUSE_H

#include <vector>

class Clause {

public:

    Clause();

    void addLiteral(int literal);

    std::vector<int> literals;
};

#endif
