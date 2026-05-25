#pragma once

#include <vector>
#include <queue>
#include <cmath>

struct Clause {
    std::vector<int> lits;

    bool learnt = false;

    float activity = 0.0f;
};

struct Variable {

    // -1 = unassigned
    // 0 = false
    // 1 = true
    int value = -1;

    int level = 0;

    Clause* reason = nullptr;

    float activity = 0.0f;

    bool seen = false;

    int polarity = 1;
};

struct TrailEntry {

    int lit = 0;

    int level = 0;

    Clause* reason = nullptr;
};

struct SolverState {

    // VARIABLES
    std::vector<Variable> vars;

    // CLAUSES
    std::vector<Clause> clauses;

    // ASSIGNMENT TRAIL
    std::vector<TrailEntry> trail;

    // PROPAGATION
    std::queue<int> propagationQueue;

    // WATCH LITERALS
    std::vector<std::vector<int>> watchLists;

    // DECISION LEVEL
    int currentLevel = 0;

    // STATISTICS
    int conflicts = 0;

    int decisions = 0;

    int propagations = 0;

    int restarts = 0;

    // VSIDS
    float varDecay = 0.95f;

    float clauseDecay = 0.999f;
};
