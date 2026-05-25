#include "solver_state.h"

static int restartLimit =
    100;

static int restartMultiplier =
    2;

bool shouldRestart(
    SolverState& s) {

    return
        s.conflicts >=
        restartLimit;
}

void performRestart(
    SolverState& s) {

    if (s.currentLevel <= 0)
        return;

    while (
        !s.trail.empty()) {

        auto t =
            s.trail.back();

        int v =
            abs(t.lit);

        s.vars[v].value =
            -1;

        s.vars[v].reason =
            -1;

        s.vars[v].level =
            -1;

        s.trail.pop_back();
    }

    while (
        !s.propagationQueue
            .empty()) {

        s.propagationQueue
            .pop();
    }

    s.currentLevel = 0;

    s.restarts++;

    restartLimit *=
        restartMultiplier;

    if (restartLimit >
        500000) {

        restartLimit =
            500000;
    }
}

void reduceLearnedClauses(
    SolverState& s) {

    int learnedCount = 0;

    for (auto& c : s.clauses) {

        if (c.learned &&
            !c.removed) {

            learnedCount++;
        }
    }

    if (learnedCount < 200)
        return;

    int removed = 0;

    for (auto& c : s.clauses) {

        if (!c.learned)
            continue;

        if (c.removed)
            continue;

        if (c.lits.size() <= 2)
            continue;

        if (c.activity > 2.0f)
            continue;

        c.removed = true;

        removed++;

        if (removed >=
            learnedCount / 3) {

            break;
        }
    }
}

void bumpClauseActivity(
    Clause& c) {

    c.activity += 1.0f;

    if (c.activity >
        1e20f) {

        c.activity *=
            1e-20f;
    }
}

void decayClauseActivities(
    SolverState& s) {

    for (auto& c : s.clauses) {

        c.activity *=
            s.clauseDecay;
    }
}
