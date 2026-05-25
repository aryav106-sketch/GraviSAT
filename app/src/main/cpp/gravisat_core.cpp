#include "solver.h"

void initSolver(SolverState& state) {

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

    state.varDecay = 0.95f;
    state.clauseDecay = 0.999f;
}
