#ifndef GRAVISAT_SOLVER_STATE_H
#define GRAVISAT_SOLVER_STATE_H

#include <vector>
#include <cstdint>

struct Variable {

    int value;
    bool seen;

    Variable() {
        value = -1;
        seen = false;
    }
};

struct SolverState {

    int vars;
    int clauses;

    int currentLevel;

    std::vector<Variable> vars_data;

    std::vector<int> assignment;
    std::vector<int> trail;
    std::vector<int> decision_level;

    SolverState() {

        vars = 0;
        clauses = 0;
        currentLevel = 0;
    }
};

#endif
