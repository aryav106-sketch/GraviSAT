#include "solver_state.h"

void analyzeConflict(SolverState& s) {

    for (size_t i = 0; i < s.vars_data.size(); i++) {

        s.vars_data[i].seen = false;
    }

    s.currentLevel++;
}
