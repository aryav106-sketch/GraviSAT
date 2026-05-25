#include "solver_state.h"

#include <cmath>

bool shouldRestart(
    SolverState& s,
    int limit) {

    return s.conflicts >= limit;
}

void clearAssignments(
    SolverState& s) {

    for (auto& v : s.vars) {

        v.value = -1;

        v.level = 0;

        v.reason = nullptr;
    }

    s.trail.clear();

    while (!s.propagationQueue.empty()) {
        s.propagationQueue.pop();
    }

    s.currentLevel = 0;
}

void performRestart(
    SolverState& s) {

    clearAssignments(s);

    s.restarts++;
}

void decayClauseActivities(
    SolverState& s) {

    for (auto& c : s.clauses) {
        c.activity *= s.clauseDecay;
    }
}
