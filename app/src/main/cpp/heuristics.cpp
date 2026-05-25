#include "solver_state.h"

#include <cmath>
#include <cfloat>

void bumpVariableActivity(
    SolverState& s,
    int var) {

    if (var < 0 ||
        var >= s.vars.size()) {
        return;
    }

    s.vars[var].activity += 1.0f;

    if (s.vars[var].activity > FLT_MAX / 1000.0f) {

        for (auto& v : s.vars) {
            v.activity *= 1e-6f;
        }
    }
}

void decayVariableActivities(
    SolverState& s) {

    for (auto& v : s.vars) {
        v.activity *= s.varDecay;
    }
}

int pickBranchVariable(
    SolverState& s) {

    int bestVar = -1;

    float bestScore = -1.0f;

    for (int v = 0;
         v < s.vars.size();
         v++) {

        if (s.vars[v].value != -1) {
            continue;
        }

        if (s.vars[v].activity >
            bestScore) {

            bestScore =
                s.vars[v].activity;

            bestVar = v;
        }
    }

    if (bestVar != -1) {
        s.decisions++;
    }

    return bestVar;
}

void resetVariableActivities(
    SolverState& s) {

    for (auto& v : s.vars) {
        v.activity = 0.0f;
    }
}
