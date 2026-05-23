#include <jni.h>
#include <string>
#include <vector>
#include <sstream>
#include <queue>
#include <cmath>
#include <algorithm>

struct Variable {

    int value;

    int level;

    int reason;

    int polarity;
};

struct Clause {

    std::vector<int> lits;

    int watch1;

    int watch2;

    double activity;

    bool learned;

    bool removed;
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

    Clause makeClause(
            const std::vector<int>& lits,
            bool learned = false) {

        Clause c;

        c.lits = lits;

        c.watch1 = 0;

        c.watch2 =
            (lits.size() > 1) ? 1 : 0;

        c.activity = 0.0;

        c.learned = learned;

        c.removed = false;

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

        int var = std::abs(lit);

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

            int watchPos =
                (clause.lits[
                    clause.watch1] == -lit)
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

        int highest = 0;

        int secondHighest = 0;

        for (int lit :
             conflictClause->lits) {

            int var =
                std::abs(lit);

            int lvl =
                vars[var].level;

            learned.push_back(-lit);

            bumpVariableActivity(var);

            if (lvl > highest) {

                secondHighest =
                    highest;

                highest = lvl;
            }
            else if (
                lvl > secondHighest &&
                lvl != highest) {

                secondHighest = lvl;
            }
        }

        backtrackLevel =
            secondHighest;

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
    }

    void reduceLearnedClauses() {

        if (clauses.size() < 200)
            return;

        std::sort(
            clauses.begin(),
            clauses.end(),
            [](const Clause& a,
               const Clause& b) {

                return
                    a.activity <
                    b.activity;
            });

        int removeCount =
            clauses.size() / 5;

        for (int i = 0;
             i < removeCount;
             i++) {

            if (clauses[i].learned) {

                clauses[i].removed = true;
            }
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

        restartLimit += 100;
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

                int backtrackLevel;

                std::vector<int>
                learned =
                    analyzeConflict(
                        conflict,
                        backtrackLevel);

                addLearnedClause(
                    learned);

                backtrack(
                    backtrackLevel);

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

        out << "Decision Level: "
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
