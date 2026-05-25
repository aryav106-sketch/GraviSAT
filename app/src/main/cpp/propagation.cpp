#include "solver_state.h"

#include <cmath>

static int literalValue(
    SolverState& s,
    int lit) {

    int v = std::abs(lit);

    if (v >= s.vars.size()) {
        return -1;
    }

    int val = s.vars[v].value;

    if (val == -1) {
        return -1;
    }

    if (lit > 0) {
        return val;
    }

    return 1 - val;
}

bool assignLiteral(
    SolverState& s,
    int lit,
    Clause* reason) {

    int v = std::abs(lit);

    int assign =
        (lit > 0) ? 1 : 0;

    if (s.vars[v].value != -1) {
        return s.vars[v].value == assign;
    }

    s.vars[v].value = assign;

    s.vars[v].level =
        s.currentLevel;

    s.vars[v].reason =
        reason;

    s.trail.push_back({
        lit,
        s.currentLevel,
        reason
    });

    s.propagationQueue.push(lit);

    return true;
}

Clause* propagate(
    SolverState& s) {

    while (!s.propagationQueue.empty()) {

        int lit =
            s.propagationQueue.front();

        s.propagationQueue.pop();

        for (auto& c : s.clauses) {

            bool satisfied = false;

            int unassigned = 0;

            int lastLit = 0;

            for (int x : c.lits) {

                int val =
                    literalValue(s, x);

                if (val == 1) {
                    satisfied = true;
                    break;
                }

                if (val == -1) {
                    unassigned++;
                    lastLit = x;
                }
            }

            if (satisfied) {
                continue;
            }

            if (unassigned == 0) {
                return &c;
            }

            if (unassigned == 1) {

                if (!assignLiteral(
                        s,
                        lastLit,
                        &c)) {

                    return &c;
                }
            }
        }
    }

    return nullptr;
}
