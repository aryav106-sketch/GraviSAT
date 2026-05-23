#include <jni.h>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <algorithm>

struct Assignment {

    int value;
    int level;
    int reason;
};

class GraviSAT {

private:

    int numVars;

    int conflicts;

    int decisionsCount;

    int currentLevel;

    std::vector<std::vector<int>> clauses;

    std::vector<std::vector<int>> learnedClauses;

    std::vector<Assignment> assigns;

    std::vector<double> activity;

public:

    GraviSAT() {

        numVars = 0;

        conflicts = 0;

        decisionsCount = 0;

        currentLevel = 0;
    }

    bool parseDIMACS(const std::string &input) {

        clauses.clear();

        learnedClauses.clear();

        assigns.clear();

        activity.clear();

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

                int vars, cls;

                ls >> tmp >> tmp >> vars >> cls;

                numVars = vars;

                assigns.resize(numVars + 1);

                activity.resize(numVars + 1, 0.0);

                for (int i = 1; i <= numVars; i++) {

                    assigns[i].value = -1;
                    assigns[i].level = -1;
                    assigns[i].reason = -1;
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

        int val = assigns[var].value;

        if (val == -1)
            return false;

        return (lit > 0 && val == 1) ||
               (lit < 0 && val == 0);
    }

    bool literalFalse(int lit) {

        int var = std::abs(lit);

        int val = assigns[var].value;

        if (val == -1)
            return false;

        return (lit > 0 && val == 0) ||
               (lit < 0 && val == 1);
    }

    bool propagateDatabase(
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

                int var = std::abs(lit);

                if (assigns[var].value == -1) {

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

                int var = std::abs(lastLit);

                int value = (lastLit > 0) ? 1 : 0;

                assigns[var].value = value;

                assigns[var].level = currentLevel;

                assigns[var].reason = (int)c;
            }
        }

        return true;
    }

    bool propagate() {

        bool changed = true;

        while (changed) {

            changed = false;

            size_t before =
                    learnedClauses.size();

            if (!propagateDatabase(clauses))
                return false;

            if (!propagateDatabase(learnedClauses))
                return false;

            if (learnedClauses.size() != before)
                changed = true;
        }

        return true;
    }

    void learnClause(
            const std::vector<int>& conflictClause) {

        std::vector<int> learned;

        for (int lit : conflictClause) {

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

            if (assigns[i].value == -1 &&
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

        for (int i = 1; i <= numVars; i++) {

            if (assigns[i].level > level) {

                assigns[i].value = -1;
                assigns[i].level = -1;
                assigns[i].reason = -1;
            }
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

        decisionsCount++;

        currentLevel++;

        {
            auto backup = assigns;

            assigns[var].value = 1;
            assigns[var].level = currentLevel;

            decayActivities();

            if (solveRecursive())
                return true;

            assigns = backup;
        }

        {
            auto backup = assigns;

            assigns[var].value = 0;
            assigns[var].level = currentLevel;

            decayActivities();

            if (solveRecursive())
                return true;

            assigns = backup;
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

                if (assigns[i].value == 1)
                    out << "TRUE";
                else
                    out << "FALSE";

                out << "\n";
            }
        }

        out << "\n";
        out << "Decisions: "
            << decisionsCount << "\n";

        out << "Conflicts: "
            << conflicts << "\n";

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
