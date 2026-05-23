#include <jni.h>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <queue>

struct VariableState {

    int value;
    int level;
    int reason;
};

struct TrailEntry {

    int variable;
    int value;
    int level;
};

class GraviSAT {

private:

    int numVars;

    int conflicts;

    int decisions;

    int propagations;

    int currentLevel;

    std::vector<std::vector<int>> clauses;

    std::vector<std::vector<int>> learnedClauses;

    std::vector<VariableState> vars;

    std::vector<double> activity;

    std::vector<TrailEntry> trail;

    std::queue<int> propagationQueue;

public:

    GraviSAT() {

        numVars = 0;

        conflicts = 0;

        decisions = 0;

        propagations = 0;

        currentLevel = 0;
    }

    bool parseDIMACS(const std::string &input) {

        clauses.clear();

        learnedClauses.clear();

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

                ls >> tmp >> tmp >> numVars >> cls;

                vars.resize(numVars + 1);

                activity.resize(numVars + 1, 0.0);

                for (int i = 1; i <= numVars; i++) {

                    vars[i].value = -1;
                    vars[i].level = -1;
                    vars[i].reason = -1;
                }
            }
            else {

                std::stringstream ls(line);

                int lit;

                std::vector<int> clause;

                while (ls >> lit) {

                    if (lit == 0)
                        break;

                    clause.push_back(lit);

                    activity[std::abs(lit)] += 1.0;
                }

                if (!clause.empty())
                    clauses.push_back(clause);
            }
        }

        return true;
    }

    bool literalTrue(int lit) {

        int var = std::abs(lit);

        int val = vars[var].value;

        if (val == -1)
            return false;

        return (lit > 0 && val == 1) ||
               (lit < 0 && val == 0);
    }

    bool literalFalse(int lit) {

        int var = std::abs(lit);

        int val = vars[var].value;

        if (val == -1)
            return false;

        return (lit > 0 && val == 0) ||
               (lit < 0 && val == 1);
    }

    bool assignLiteral(
            int lit,
            int level,
            int reason) {

        int var = std::abs(lit);

        int value = (lit > 0) ? 1 : 0;

        if (vars[var].value != -1) {

            return vars[var].value == value;
        }

        vars[var].value = value;

        vars[var].level = level;

        vars[var].reason = reason;

        trail.push_back({var, value, level});

        propagationQueue.push(var);

        propagations++;

        return true;
    }

    bool propagateClauseDatabase(
            std::vector<std::vector<int>>& db) {

        for (size_t c = 0; c < db.size(); c++) {

            auto &clause = db[c];

            bool satisfied = false;

            int unassigned = 0;

            int lastLit = 0;

            for (int lit : clause) {

                if (literalTrue(lit)) {

                    satisfied = true;

                    break;
                }

                if (!literalFalse(lit)) {

                    unassigned++;

                    lastLit = lit;
                }
            }

            if (satisfied)
                continue;

            if (unassigned == 0) {

                conflicts++;

                learnClause(clause);

                return false;
            }

            if (unassigned == 1) {

                if (!assignLiteral(
                        lastLit,
                        currentLevel,
                        (int)c)) {

                    conflicts++;

                    return false;
                }
            }
        }

        return true;
    }

    bool propagate() {

        while (!propagationQueue.empty()) {

            propagationQueue.pop();

            if (!propagateClauseDatabase(clauses))
                return false;

            if (!propagateClauseDatabase(
                    learnedClauses))
                return false;
        }

        return true;
    }

    void learnClause(
            const std::vector<int>& conflict) {

        std::vector<int> learned;

        for (int lit : conflict) {

            learned.push_back(-lit);

            activity[std::abs(lit)] += 5.0;
        }

        learnedClauses.push_back(learned);
    }

    bool allSatisfied() {

        for (auto &clause : clauses) {

            bool sat = false;

            for (int lit : clause) {

                if (literalTrue(lit)) {

                    sat = true;

                    break;
                }
            }

            if (!sat)
                return false;
        }

        return true;
    }

    int chooseVariable() {

        double best = -1.0;

        int bestVar = -1;

        for (int i = 1; i <= numVars; i++) {

            if (vars[i].value == -1 &&
                activity[i] > best) {

                best = activity[i];

                bestVar = i;
            }
        }

        return bestVar;
    }

    void decayActivities() {

        for (int i = 1; i <= numVars; i++) {

            activity[i] *= 0.95;
        }
    }

    void backtrack(int level) {

        while (!trail.empty()) {

            auto entry = trail.back();

            if (entry.level <= level)
                break;

            vars[entry.variable].value = -1;
            vars[entry.variable].level = -1;
            vars[entry.variable].reason = -1;

            trail.pop_back();
        }

        currentLevel = level;
    }

    bool solveRecursive() {

        if (!propagate())
            return false;

        if (allSatisfied())
            return true;

        int var = chooseVariable();

        if (var == -1)
            return false;

        decisions++;

        currentLevel++;

        {
            auto backupVars = vars;
            auto backupTrail = trail;

            assignLiteral(var, currentLevel, -1);

            decayActivities();

            if (solveRecursive())
                return true;

            vars = backupVars;
            trail = backupTrail;
        }

        {
            auto backupVars = vars;
            auto backupTrail = trail;

            assignLiteral(-var, currentLevel, -1);

            decayActivities();

            if (solveRecursive())
                return true;

            vars = backupVars;
            trail = backupTrail;
        }

        backtrack(currentLevel - 1);

        return false;
    }

    std::string solveSAT() {

        bool sat = solveRecursive();

        std::stringstream out;

        if (!sat) {

            out << "UNSATISFIABLE\n\n";
        }
        else {

            out << "SATISFIABLE\n\n";

            for (int i = 1; i <= numVars; i++) {

                out << "x" << i << " = ";

                if (vars[i].value == 1)
                    out << "TRUE";
                else
                    out << "FALSE";

                out << "\n";
            }
        }

        out << "\n";
        out << "Decisions: "
            << decisions << "\n";

        out << "Conflicts: "
            << conflicts << "\n";

        out << "Propagations: "
            << propagations << "\n";

        out << "Learned Clauses: "
            << learnedClauses.size() << "\n";

        out << "Decision Level: "
            << currentLevel << "\n";

        return out.str();
    }
};

extern "C"
JNIEXPORT jstring JNICALL
Java_com_gravisat_app_MainActivity_solveSAT(
        JNIEnv *env,
        jobject thiz,
        jstring inputText) {

    const char *raw =
            env->GetStringUTFChars(inputText, 0);

    std::string dimacs(raw);

    env->ReleaseStringUTFChars(inputText, raw);

    GraviSAT solver;

    solver.parseDIMACS(dimacs);

    std::string result =
            solver.solveSAT();

    return env->NewStringUTF(result.c_str());
}
