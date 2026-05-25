#include "solver.h"

void clearSeenFlags(SolverState& s) {

    for (size_t i = 0; i < s.vars.size(); i++) {

        s.vars[i].seen = false;
    }
}
