#ifndef SOLVER_H
#define SOLVER_H

#include <vector>
#include <string>

struct Clause {

    std::vector<int> literals;
};

struct Variable {

    int value = -1;
};

struct SolverState {

    std::vector<Variable> vars;

    std::vector<Clause> clauses;

    int conflicts = 0;

    int decisions = 0;
};

#endif
