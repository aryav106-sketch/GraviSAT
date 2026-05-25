#include "solver_state.h"

static float activityIncrement =
    1.0f;

static inline void rescaleActivities(
    SolverState& s) {

    for (auto& v : s.vars) {

        v.activity *=
            1e-20f;
    }

    activityIncrement *=
        1e-20f;
}

void bumpVariableActivity(
    SolverState& s,
    int var) {

    if (var <= 0 ||
        var >= s.vars.size())
        return;

    s.vars[var]
        .activity +=
            activityIncrement;

    if (s.vars[var]
            .activity >
        1e100f) {

        rescaleActivities(
            s);
    }
}

void decayActivities(
    SolverState& s) {

    activityIncrement /=
        s.varDecay;
}

int pickBranchLiteral(
    SolverState& s) {

    float bestScore =
        -1.0f;

    int bestVar = -1;

    for (int v = 1;
         v < s.vars.size();
         v++) {

        if (s.vars[v]
                .value != -1)
            continue;

        if (s.vars[v]
                .activity >
            bestScore) {

            bestScore =
                s.vars[v]
                    .activity;

            bestVar = v;
        }
    }

    if (bestVar == -1)
        return 0;

    s.decisions++;

    if (s.vars[bestVar]
            .polarity > 0) {

        return bestVar;
    }

    return -bestVar;
}

void updatePhaseSaving(
    SolverState& s,
    int lit) {

    int v =
        abs(lit);

    if (v <= 0 ||
        v >= s.vars.size())
        return;

    s.vars[v]
        .polarity =
            lit > 0 ? 1 : -1;
}

bool allAssigned(
    SolverState& s) {

    for (int v = 1;
         v < s.vars.size();
         v++) {

        if (s.vars[v]
                .value == -1) {

            return false;
        }
    }

    return true;
}

void initializeVariableActivities(
    SolverState& s) {

    for (auto& v : s.vars) {

        v.activity = 0.0f;
    }

    for (auto& c : s.clauses) {

        if (c.removed)
            continue;

        for (int lit : c.lits) {

            int v =
                abs(lit);

            if (v <= 0 ||
                v >= s.vars.size())
                continue;

            s.vars[v]
                .activity +=
                    1.0f;
        }
    }
}
