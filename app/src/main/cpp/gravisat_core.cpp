#include <jni.h>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <algorithm>

class GraviSAT {

private:

    int numVars;

    std::vector<std::vector<int>> clauses;

    std::vector<int> assignment;

    std::vector<double> activity;

public:

    GraviSAT() {
        numVars = 0;
    }

    bool parseDIMACS(const std::string &input) {

        clauses.clear();

        assignment.clear();

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

                assignment.resize(numVars + 1, -1);

                activity.resize(numVars + 1, 0.0);
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

        int val = assignment[var];

        if (val == -1)
            return false;

        return (lit > 0 && val == 1) ||
               (lit < 0 && val == 0);
    }

    bool clauseSatisfied(const std::vector<int>& clause) {

        for (int lit : clause) {

            if (literalTrue(lit))
                return true;
        }

        return false;
    }

    bool hasConflict() {

        for (auto &clause : clauses) {

            bool sat = false;

            bool undecided = false;

            for (int lit : clause) {

                int var = std::abs(lit);

                int val = assignment[var];

                if (val == -1)
                    undecided = true;

                if (literalTrue(lit)) {

                    sat = true;

                    break;
                }
            }

            if (!sat && !undecided)
                return true;
        }

        return false;
    }

    bool allSatisfied() {

        for (auto &clause : clauses) {

            if (!clauseSatisfied(clause))
                return false;
        }

        return true;
    }

    bool watchedPropagation() {

        bool changed = true;

        while (changed) {

            changed = false;

            for (auto &clause : clauses) {

                if (clauseSatisfied(clause))
                    continue;

                int unassigned = 0;

                int lastLit = 0;

                for (int lit : clause) {

                    int var = std::abs(lit);

                    if (assignment[var] == -1) {

                        unassigned++;

                        lastLit = lit;
                    }
                }

                if (unassigned == 0)
                    return false;

                if (unassigned == 1) {

                    int var = std::abs(lastLit);

                    int value = (lastLit > 0) ? 1 : 0;

                    if (assignment[var] == -1) {

                        assignment[var] = value;

                        changed = true;
                    }
                    else if (assignment[var] != value) {

                        return false;
                    }
                }
            }
        }

        return true;
    }

    int chooseVSIDSVariable() {

        double bestScore = -1.0;

        int bestVar = -1;

        for (int i = 1; i <= numVars; i++) {

            if (assignment[i] == -1 &&
                activity[i] > bestScore) {

                bestScore = activity[i];

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

    bool solveRecursive() {

        if (!watchedPropagation())
            return false;

        if (hasConflict())
            return false;

        if (allSatisfied())
            return true;

        int var = chooseVSIDSVariable();

        if (var == -1)
            return false;

        std::vector<int> backup = assignment;

        assignment[var] = 1;

        decayActivities();

        if (solveRecursive())
            return true;

        assignment = backup;

        assignment[var] = 0;

        decayActivities();

        if (solveRecursive())
            return true;

        assignment = backup;

        return false;
    }

    std::string solveSAT() {

        bool sat = solveRecursive();

        if (!sat)
            return "UNSATISFIABLE";

        std::string out = "SATISFIABLE\n\n";

        for (int i = 1; i <= numVars; i++) {

            out += "x";

            out += std::to_string(i);

            out += " = ";

            if (assignment[i] == 1)
                out += "TRUE";
            else
                out += "FALSE";

            out += "\n";
        }

        return out;
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
