#include <jni.h>
#include <string>
#include <vector>
#include <sstream>
#include <queue>
#include <cmath>
#include <algorithm>
#include <set>

struct Variable {

    int value;

    int level;

    int reason;

    int polarity;

    bool seen;
};

struct Clause {

    std::vector<int> lits;

    int watch1;

    int watch2;

    double activity;

    int lbd;

    bool learned;

    bool removed;

    bool binary;
};

struct TrailEntry {

    int lit;

    int level;

    int reason;
};

class GraviSAT {

private:

    int numVars;

    int conflicts;

    int decisions;

    int propagations;

    int restarts;

    int currentLevel;

    int restartLimit;

    double varInc;

    double varDecay;

    double clauseInc;

    double clauseDecay;

    std::vector<Variable> vars;

    std::vector<double> activity;

    std::vector<Clause> clauses;

    std::vector<TrailEntry> trail;

    std::queue<int> propagationQueue;

    std::vector<std::vector<int>> watchLists;

public:

    GraviSAT() {

        numVars = 0;

        conflicts = 0;

        decisions = 0;

        propagations = 0;

        restarts = 0;

        currentLevel = 0;

        restartLimit = 100;

        varInc = 1.0;

        varDecay = 0.95;

        clauseInc = 1.0;

        clauseDecay = 0.999;
    }

    int litIndex(int lit) {

        if (lit > 0)
            return lit * 2;

        return (-lit * 2) + 1;
    }

    int computeLBD(
            const std::vector<int>& lits) {

        std::set<int> levels;

        for (int lit : lits) {

            int var =
                std::abs(lit);

            levels.insert(
                vars[var].level);
        }

        return levels.size();
    }

    Clause makeClause(
            const std::vector<int>& lits,
            bool learned = false) {

        Clause c;

        c.lits = lits;

        c.watch1 = 0;

        c.watch2 =
            (lits.size() > 1)
            ? 1
            : 0;

        c.activity = 0.0;

        c.lbd =
            computeLBD(lits);

        c.learned = learned;

        c.removed = false;

        c.binary =
            lits.size() == 2;

        return c;
    }

    void addWatch(
            int lit,
            int clauseId) {

        watchLists[
            litIndex(lit)].push_back(
                clauseId);
    }

    void bumpVariableActivity(
            int var) {

        activity[var] += varInc;

        if (activity[var] > 1e100) {

            for (int i = 1;
                 i <= numVars;
                 i++) {

                activity[i] *= 1e-100;
            }

            varInc *= 1e-100;
        }
    }

    void decayVariableActivity() {

        varInc /= varDecay;
    }

    void bumpClauseActivity(
            Clause& c) {

        c.activity += clauseInc;
    }

    void decayClauseActivity() {

        clauseInc /= clauseDecay;
    }

    bool parseDIMACS(
            const std::string& input) {

        clauses.clear();

        vars.clear();

        activity.clear();

        trail.clear();

        std::stringstream ss(input);

        std::string line;

        while (std::getline(ss, line)) {

            if (line.empty())
                continue;

            if (line[0] == 'c')
                continue;

            if (line[0] == 'p') {

                std::stringstream ls(line);

                std::string tmp;

                int cls;

                ls >> tmp
                   >> tmp
                   >> numVars
                   >> cls;

                vars.resize(numVars + 1);

                activity.resize(
                    numVars + 1,
                    0.0);

                watchLists.resize(
                    (numVars + 1) * 2 + 2);

                for (int i = 1;
                     i <= numVars;
                     i++) {

                    vars[i].value = -1;

                    vars[i].level = -1;

                    vars[i].reason = -1;

                    vars[i].polarity = 1;

                    vars[i].seen = false;
                }
            }
            else {

                std::stringstream ls(line);

                int lit;

                std::vector<int> lits;

                while (ls >> lit) {

                    if (lit == 0)
                        break;

                    lits.push_back(lit);

                    activity[
                        std::abs(lit)] += 1.0;
                }

                if (!lits.empty()) {

                    Clause c =
                        makeClause(
                            lits,
                            false);

                    int id =
                        clauses.size();

                    clauses.push_back(c);

                    addWatch(
                        lits[c.watch1],
                        id);

                    addWatch(
                        lits[c.watch2],
                        id);
                }
            }
        }

        return true;
    }

    bool literalTrue(int lit) {

        int var = std::abs(lit);

        int val = vars[var].value;

        if (val == -1)
            return false;

        return
            (lit > 0 && val == 1) ||
            (lit < 0 && val == 0);
    }

    bool literalFalse(int lit) {

        int var = std::abs(lit);

        int val = vars[var].value;

        if (val == -1)
            return false;

        return
            (lit > 0 && val == 0) ||
            (lit < 0 && val == 1);
    }

    bool enqueue(
            int lit,
            int level,
            int reason) {

        int var =
            std::abs(lit);

        int value =
            (lit > 0) ? 1 : 0;

        if (vars[var].value != -1) {

            return
                vars[var].value == value;
        }

        vars[var].value = value;

        vars[var].level = level;

        vars[var].reason = reason;

        vars[var].polarity = value;

        trail.push_back({
            lit,
            level,
            reason
        });

        propagationQueue.push(lit);

        propagations++;

        return true;
    }

    Clause* propagateLiteral(
            int lit) {

        int idx =
            litIndex(-lit);

        auto watchCopy =
            watchLists[idx];

        for (int clauseId :
             watchCopy) {

            Clause& clause =
                clauses[clauseId];

            if (clause.removed)
                continue;

            if (clause.binary) {

                int a =
                    clause.lits[0];

                int b =
                    clause.lits[1];

                int other =
                    (a == -lit)
                    ? b
                    : a;

                if (literalFalse(other)) {

                    conflicts++;

                    return &clause;
                }

                if (!literalTrue(other)) {

                    if (!enqueue(
                            other,
                            currentLevel,
                            clauseId)) {

                        conflicts++;

                        return &clause;
                    }
                }

                continue;
            }

            int watchPos =
                (clause.lits[
                    clause.watch1]
                    == -lit)
                ? clause.watch1
                : clause.watch2;

            int otherPos =
                (watchPos ==
                 clause.watch1)
                ? clause.watch2
                : clause.watch1;

            int otherLit =
                clause.lits[otherPos];

            if (literalTrue(otherLit))
                continue;

            bool found = false;

            for (size_t i = 0;
                 i < clause.lits.size();
                 i++) {

                if ((int)i ==
                        clause.watch1 ||
                    (int)i ==
                        clause.watch2)
                    continue;

                int newLit =
                    clause.lits[i];

                if (!literalFalse(
                        newLit)) {

                    if (watchPos ==
                        clause.watch1)
                        clause.watch1 = i;
                    else
                        clause.watch2 = i;

                    addWatch(
                        newLit,
                        clauseId);

                    found = true;

                    break;
                }
            }

            if (found)
                continue;

            if (literalFalse(
                    otherLit)) {

                conflicts++;

                bumpClauseActivity(
                    clause);

                return &clause;
            }

            if (!enqueue(
                    otherLit,
                    currentLevel,
                    clauseId)) {

                conflicts++;

                return &clause;
            }
        }

        return nullptr;
    }

    Clause* propagate() {

        while (
            !propagationQueue.empty()) {

            int lit =
                propagationQueue.front();

            propagationQueue.pop();

            Clause* conflict =
                propagateLiteral(lit);

            if (conflict != nullptr)
                return conflict;
        }

        return nullptr;
    }

 std::vector<int> analyzeConflict(
        Clause* conflictClause,
        int& backtrackLevel) {

    std::vector<int> learned;

    learned.push_back(0);

    int pathC = 0;

    int p = 0;

    int index =
        trail.size() - 1;

    Clause* clause =
        conflictClause;

    do {

        bumpClauseActivity(
            *clause);

        for (int lit :
             clause->lits) {

            int var =
                std::abs(lit);

            if (var == std::abs(p))
                continue;

            if (!vars[var].seen &&
                vars[var].level > 0) {

                vars[var].seen = true;

                bumpVariableActivity(
                    var);

                if (vars[var].level ==
                    currentLevel) {

                    pathC++;
                }
                else {

                    learned.push_back(
                        lit);
                }
            }
        }

        while (true) {

            p =
                trail[index--].lit;

            if (vars[
                    std::abs(p)].seen)
                break;
        }

        vars[
            std::abs(p)].seen = false;

        pathC--;

        if (pathC > 0) {

            int reason =
                vars[
                    std::abs(p)]
                    .reason;

            if (reason >= 0)
                clause =
                    &clauses[
                        reason];
        }

    } while (pathC > 0);

    learned[0] = -p;

    // =========================================
    // REAL CLAUSE MINIMIZATION
    // =========================================

    std::vector<int> minimized;

    minimized.push_back(
        learned[0]);

    for (size_t i = 1;
         i < learned.size();
         i++) {

        int lit = learned[i];

        int var =
            std::abs(lit);

        bool removable = false;

        int reason =
            vars[var].reason;

        if (reason >= 0) {

            Clause& rc =
                clauses[reason];

            removable = true;

            for (int rlit :
                 rc.lits) {

                int rvar =
                    std::abs(rlit);

                if (rvar == var)
                    continue;

                if (!vars[rvar].seen &&
                    vars[rvar].level > 0) {

                    removable = false;

                    break;
                }
            }
        }

        if (!removable) {

            minimized.push_back(
                lit);
        }
    }

    learned = minimized;

    // =========================================
    // BACKTRACK LEVEL
    // =========================================

    int maxLevel = 0;

    for (size_t i = 1;
         i < learned.size();
         i++) {

        int lvl =
            vars[
                std::abs(
                    learned[i])]
                .level;

        if (lvl > maxLevel)
            maxLevel = lvl;
    }

    backtrackLevel =
        maxLevel;

    // =========================================
    // CLEANUP
    // =========================================

    for (int lit : learned) {

        vars[
            std::abs(lit)]
            .seen = false;
    }

    return learned;
 }

    void addLearnedClause(
            const std::vector<int>& lits) {

        Clause c =
            makeClause(
                lits,
                true);

        int id =
            clauses.size();

        clauses.push_back(c);

        addWatch(
            lits[c.watch1],
            id);

        addWatch(
            lits[c.watch2],
            id);

        enqueue(
            lits[0],
            backtrackLevel(),
            id);
    }

    int backtrackLevel() {

        return currentLevel > 0
               ? currentLevel - 1
               : 0;
    }

    void reduceLearnedClauses() {

        if (clauses.size() < 300)
            return;

        std::vector<int> order;

        for (size_t i = 0;
             i < clauses.size();
             i++) {

            if (clauses[i].learned &&
                !clauses[i].removed &&
                clauses[i].lbd > 2) {

                order.push_back(i);
            }
        }

        std::sort(
            order.begin(),
            order.end(),
            [&](int a, int b) {

                Clause& A =
                    clauses[a];

                Clause& B =
                    clauses[b];

                if (A.lbd != B.lbd)
                    return A.lbd > B.lbd;

                return
                    A.activity <
                    B.activity;
            });

        int removeCount =
            order.size() / 3;

        for (int i = 0;
             i < removeCount;
             i++) {

            clauses[
                order[i]]
                .removed = true;
        }
    }

    int chooseVariable() {

        double best = -1.0;

        int bestVar = -1;

        for (int i = 1;
             i <= numVars;
             i++) {

            if (vars[i].value == -1 &&
                activity[i] > best) {

                best = activity[i];

                bestVar = i;
            }
        }

        return bestVar;
    }

    void restartSearch() {

        restarts++;

        while (!trail.empty()) {

            auto t = trail.back();

            int var =
                std::abs(t.lit);

            vars[var].value = -1;

            vars[var].level = -1;

            vars[var].reason = -1;

            trail.pop_back();
        }

        currentLevel = 0;

        while (
            !propagationQueue.empty())
            propagationQueue.pop();

        restartLimit += 150;
    }

    void backtrack(int level) {

        while (!trail.empty()) {

            auto t = trail.back();

            if (t.level <= level)
                break;

            int var =
                std::abs(t.lit);

            vars[var].value = -1;

            vars[var].level = -1;

            vars[var].reason = -1;

            trail.pop_back();
        }

        currentLevel = level;
    }

    bool allAssigned() {

        for (int i = 1;
             i <= numVars;
             i++) {

            if (vars[i].value == -1)
                return false;
        }

        return true;
    }

    bool solve() {

        while (true) {

            Clause* conflict =
                propagate();

            if (conflict != nullptr) {

                if (currentLevel == 0)
                    return false;

                int bt;

                std::vector<int>
                learned =
                    analyzeConflict(
                        conflict,
                        bt);

                addLearnedClause(
                    learned);

                backtrack(bt);

                decayVariableActivity();

                decayClauseActivity();

                reduceLearnedClauses();

                if (conflicts >=
                    restartLimit) {

                    restartSearch();
                }

                continue;
            }

            if (allAssigned())
                return true;

            int var =
                chooseVariable();

            if (var == -1)
                return true;

            decisions++;

            currentLevel++;

            int lit =
                vars[var].polarity
                ? var
                : -var;

            enqueue(
                lit,
                currentLevel,
                -1);
        }
    }

    std::string resultString(
            bool sat) {

        std::stringstream out;

        if (sat) {

            out
                << "SATISFIABLE\n\n";

            for (int i = 1;
                 i <= numVars;
                 i++) {

                out
                    << "x"
                    << i
                    << " = ";

                if (vars[i].value == 1)
                    out << "TRUE";
                else
                    out << "FALSE";

                out << "\n";
            }
        }
        else {

            out
                << "UNSATISFIABLE\n";
        }

        out << "\n";

        out << "Conflicts: "
            << conflicts << "\n";

        out << "Decisions: "
            << decisions << "\n";

        out << "Propagations: "
            << propagations << "\n";

        out << "Restarts: "
            << restarts << "\n";

        out << "Clauses: "
            << clauses.size()
            << "\n";

        out << "Current Level: "
            << currentLevel << "\n";

        return out.str();
    }
};

extern "C"
JNIEXPORT jstring JNICALL
Java_com_gravisat_app_MainActivity_solveSAT(
        JNIEnv* env,
        jobject thiz,
        jstring inputText) {

    const char* raw =
        env->GetStringUTFChars(
            inputText,
            0);

    std::string dimacs(raw);

    env->ReleaseStringUTFChars(
        inputText,
        raw);

    GraviSAT solver;

    solver.parseDIMACS(dimacs);

    bool sat =
        solver.solve();

    std::string result =
        solver.resultString(sat);

    return env->NewStringUTF(
        result.c_str());
}
