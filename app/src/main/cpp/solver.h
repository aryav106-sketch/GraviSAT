#ifndef SOLVER_H
#define SOLVER_H

#include <vector>
#include <queue>
#include <cmath>

struct Clause;

struct Variable {

    int value = -1;

    int level = 0;

    float activity = 0.0f;

    bool seen = false;

    Clause* reason = nullptr;
};

struct TrailEntry {

    int lit;
};

struct Clause {

    std::vector<int> lits;

    float activity = 0.0f;

    bool learnt = false;
};

struct SolverState {

    std::vector<Variable> vars;

    std::vector<Clause> clauses;

    std::vector<TrailEntry> trail;

    std::vector<std::vector<int>> watchLists;

    std::queue<int> propagationQueue;

    int conflicts = 0;

    int decisions = 0;

    int restarts = 0;

    float varDecay = 0.95f;

    float clauseDecay = 0.999f;
};

#endif
