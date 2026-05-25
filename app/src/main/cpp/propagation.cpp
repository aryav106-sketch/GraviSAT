#include "solver_state.h"

static inline int litIndex(int lit) {

    int v = abs(lit);

    return lit > 0
        ? (v << 1)
        : ((v << 1) ^ 1);
}

static inline bool isTrue(
    SolverState& s,
    int lit) {

    int v = abs(lit);

    int val =
        s.vars[v].value;

    if (val == -1)
        return false;

    return lit > 0
        ? val == 1
        : val == 0;
}

static inline bool isFalse(
    SolverState& s,
    int lit) {

    int v = abs(lit);

    int val =
        s.vars[v].value;

    if (val == -1)
        return false;

    return lit > 0
        ? val == 0
        : val == 1;
}

static inline bool enqueue(
    SolverState& s,
    int lit,
    int reason) {

    int v = abs(lit);

    int assign =
        lit > 0 ? 1 : 0;

    if (s.vars[v].value != -1) {

        return s.vars[v].value == assign;
    }

    s.vars[v].value =
        assign;

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

static inline void addWatch(
    SolverState& s,
    int lit,
    int clauseIndex,
    int blocker) {

    int idx =
        litIndex(lit);

    if (idx >= s.watchLists.size()) {

        s.watchLists.resize(
            idx + 1);
    }

    s.watchLists[idx]
        .push_back({
            clauseIndex,
            blocker
        });
}

void initializeWatchers(
    SolverState& s) {

    s.watchLists.clear();

    for (int i = 0;
         i < s.clauses.size();
         i++) {

        Clause& c =
            s.clauses[i];

        if (c.removed)
            continue;

        if (c.lits.empty())
            continue;

        if (c.lits.size() == 1) {

            addWatch(
                s,
                c.lits[0],
                i,
                c.lits[0]);

            continue;
        }

        c.watch1 = 0;
        c.watch2 = 1;

        addWatch(
            s,
            c.lits[0],
            i,
            c.lits[1]);

        addWatch(
            s,
            c.lits[1],
            i,
            c.lits[0]);
    }
}

Clause* propagate(
    SolverState& s) {

    while (
        !s.propagationQueue.empty()) {

        int lit =
            s.propagationQueue.front();

        s.propagationQueue.pop();

        int idx =
            litIndex(-lit);

        if (idx < 0 ||
            idx >= s.watchLists.size()) {

            continue;
        }

        auto& watches =
            s.watchLists[idx];

        size_t writePos = 0;

        for (size_t i = 0;
             i < watches.size();
             i++) {

            Watcher w =
                watches[i];

            Clause& c =
                s.clauses[
                    w.clauseIndex
                ];

            if (c.removed)
                continue;

            int falseWatch =
                c.lits[c.watch1] == -lit
                ? c.watch1
                : c.watch2;

            int otherWatch =
                falseWatch == c.watch1
                ? c.watch2
                : c.watch1;

            int otherLit =
                c.lits[otherWatch];

            if (isTrue(
                    s,
                    otherLit)) {

                watches[writePos++] = w;

                continue;
            }

            bool found = false;

            for (int k = 0;
                 k < c.lits.size();
                 k++) {

                if (k == c.watch1 ||
                    k == c.watch2)
                    continue;

                int candidate =
                    c.lits[k];

                if (!isFalse(
                        s,
                        candidate)) {

                    if (falseWatch ==
                        c.watch1)
                        c.watch1 = k;
                    else
                        c.watch2 = k;

                    addWatch(
                        s,
                        candidate,
                        w.clauseIndex,
                        otherLit);

                    found = true;

                    break;
                }
            }

            if (found)
                continue;

            watches[writePos++] = w;

            if (isFalse(
                    s,
                    otherLit)) {

                return &c;
            }

            if (!enqueue(
                    s,
                    otherLit,
                    w.clauseIndex)) {

                return &c;
            }

            s.propagations++;
        }

        watches.resize(writePos);
    }

    return nullptr;
}
