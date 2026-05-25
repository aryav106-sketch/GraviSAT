#ifndef GRAVISAT_SOLVER_STATE_H
#define GRAVISAT_SOLVER_STATE_H

#include <vector>
#include <cstdint>

struct SolverState {

    int vars;
    int clauses;

    std::vector<int> assignment;
    std::vector<int> decision_level;
    std::vector<int> trail;

    SolverState() {
        vars = 0;
        clauses = 0;
    }
};

#endif
