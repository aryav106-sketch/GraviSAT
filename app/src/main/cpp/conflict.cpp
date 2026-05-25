#include "solver_state.h"

static inline int varOf(int lit) {

    return abs(lit);
}

static inline int decisionLevel(
    SolverState& s,
    int lit) {

    return s.vars[
        abs(lit)
    ].level;
}

static inline void bumpActivity(
    SolverState& s,
    int var) {

    s.vars[var]
        .activity += 1.0f;

    if (s.vars[var]
            .activity > 1e20f) {

        for (auto& v : s.vars) {

            v.activity *=
                1e-20f;
        }
    }
}

void backtrack(
    SolverState& s,
    int targetLevel) {

    while (
        !s.trail.empty()) {

        auto t =
            s.trail.back();

        if (t.level <=
            targetLevel)
            break;

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

    s.currentLevel =
        targetLevel;
}

std::vector<int>
analyzeConflict(
    SolverState& s,
    Clause* conflictClause,
    int& outBacktrackLevel) {

    std::vector<int>
        learned;

    if (conflictClause == nullptr) {

        outBacktrackLevel = 0;

        return learned;
    }

    for (auto& v : s.vars) {

        v.seen = false;
    }

    int pathC = 0;

    int p = 0;

    learned.push_back(0);

    int index =
        (int)s.trail.size() - 1;

    Clause* c =
        conflictClause;

    do {

        for (int lit : c->lits) {

            int v =
                varOf(lit);

            if (!s.vars[v].seen &&
                decisionLevel(
                    s,
                    lit) > 0) {

                s.vars[v].seen =
                    true;

                bumpActivity(
                    s,
                    v);

                if (
                    decisionLevel(
                        s,
                        lit) ==
                    s.currentLevel) {

                    pathC++;
                }
                else {

                    learned
                        .push_back(
                            lit);
                }
            }
        }

        while (true) {

            p =
                s.trail[index]
                    .lit;

            index--;

            if (
                s.vars[
                    abs(p)
                ].seen)
                break;
        }

        s.vars[
            abs(p)
        ].seen = false;

        pathC--;

        int reason =
            s.vars[
                abs(p)
            ].reason;

        if (reason == -1)
            c = nullptr;
        else
            c = &s.clauses[
                reason];

    } while (
        pathC > 0);

    learned[0] = -p;

    int maxLevel = 0;

    for (size_t i = 1;
         i < learned.size();
         i++) {

        int lvl =
            decisionLevel(
                s,
                learned[i]);

        if (lvl >
            maxLevel) {

            maxLevel =
                lvl;
        }
    }

    outBacktrackLevel =
        maxLevel;

    return learned;
}

int addLearnedClause(
    SolverState& s,
    const std::vector<int>& lits) {

    if (lits.empty())
        return -1;

    Clause c(
        lits,
        true);

    int idx =
        s.clauses.size();

    s.clauses.push_back(c);

    Clause& ref =
        s.clauses[idx];

    if (ref.lits.size() >= 1) {

        int litA =
            ref.lits[
                ref.watch1];

        int blocker =
            ref.lits[
                ref.watch2];

        int wi =
            abs(litA) << 1;

        if (litA < 0)
            wi ^= 1;

        if (wi >=
            s.watchLists.size()) {

            s.watchLists.resize(
                wi + 1);
        }

        s.watchLists[wi]
            .push_back({
                idx,
                blocker
            });
    }

    if (ref.lits.size() >= 2) {

        int litB =
            ref.lits[
                ref.watch2];

        int blocker =
            ref.lits[
                ref.watch1];

        int wi =
            abs(litB) << 1;

        if (litB < 0)
            wi ^= 1;

        if (wi >=
            s.watchLists.size()) {

            s.watchLists.resize(
                wi + 1);
        }

        s.watchLists[wi]
            .push_back({
                idx,
                blocker
            });
    }

    return idx;
}
