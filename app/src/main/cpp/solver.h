#pragma once

#include <vector>
#include <queue>
#include <cstdint>

struct Clause {
    std::vector<int> lits;
    bool learned = false;
    float activity = 0.0f;
};

struct Variable {
    int value = -1;     
    int level = 0;
    int reason = -1;

    float activity = 0.0f;

    bool polarity = true;
    bool seen = false;
};

struct TrailEntry {
    int lit;
    int level;
    int reason;
};

struct SolverState {

    // VARIABLES
    std::vector<Variable> vars;

    // CLAUSES
    std::vector<Clause> clauses;

    // ASSIGNMENT TRAIL
    std::vector<TrailEntry> trail;

    // WATCH LISTS
    std::vector<std::vector<int>> watchLists;

    // PROPAGATION QUEUE
    std::queue<int> propagationQueue;

    // STATS
    int conflicts = 0;
    int decisions = 0;
    int restarts = 0;

    // HEURISTICS
    float varDecay = 0.95f;
    float clauseDecay = 0.999f;

    // DECISION LEVEL
    int currentLevel = 0;
};
