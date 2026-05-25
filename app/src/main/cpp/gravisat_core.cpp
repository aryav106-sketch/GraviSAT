#include "solver.h"

class GraviSATCore {
public:

    SolverState state;

    GraviSATCore() {

        state.vars.clear();
        state.clauses.clear();
        state.trail.clear();
        state.watchLists.clear();

        while (!state.propagationQueue.empty()) {
            state.propagationQueue.pop();
        }

        state.conflicts = 0;
        state.decisions = 0;
        state.restarts = 0;

        state.currentLevel = 0;
    }

    int getResult() {

        return 777;
    }
};
