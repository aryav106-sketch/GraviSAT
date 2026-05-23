#include <jni.h>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <cmath>

class GraviSAT {

private:

    int numVars;

    std::vector<std::vector<int>> clauses;

    std::vector<int> assignment;

public:

    GraviSAT() {
        numVars = 0;
    }

    bool parseDIMACS(const std::string &input) {

        clauses.clear();

        assignment.clear();

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
            }
            else {

                std::stringstream ls(line);

                int lit;

                std::vector<int> clause;

                while (ls >> lit) {

                    if (lit == 0)
                        break;

                    clause.push_back(lit);
                }

                if (!clause.empty())
                    clauses.push_back(clause);
            }
        }

        return true;
    }

    bool clauseSatisfied(const std::vector<int>& clause) {

        for (int lit : clause) {

            int var = std::abs(lit);

            int val = assignment[var];

            if ((lit > 0 && val == 1) ||
                (lit < 0 && val == 0)) {

                return true;
            }
        }

        return false;
    }

    bool hasConflict() {

        for (auto &clause : clauses) {

            bool satisfied = false;

            bool undecided = false;

            for (int lit : clause) {

                int var = std::abs(lit);

                int val = assignment[var];

                if (val == -1)
                    undecided = true;

                if ((lit > 0 && val == 1) ||
                    (lit < 0 && val == 0)) {

                    satisfied = true;

                    break;
                }
            }

            if (!satisfied && !undecided)
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

    bool unitPropagation() {

        bool changed = true;

        while (changed) {

            changed = false;

            for (auto &clause : clauses) {

                if (clauseSatisfied(clause))
                    continue;

                int unassignedCount = 0;

                int lastLit = 0;

                for (int lit : clause) {

                    int var = std::abs(lit);

                    if (assignment[var] == -1) {

                        unassignedCount++;

                        lastLit = lit;
                    }
                }

                if (unassignedCount == 1) {

                    int var = std::abs(lastLit);

                    assignment[var] = (lastLit > 0) ? 1 : 0;

                    changed = true;

                    if (hasConflict())
                        return false;
                }
            }
        }

        return true;
    }

    void pureLiteralElimination() {

        std::vector<int> polarity(numVars + 1, 0);

        for (auto &clause : clauses) {

            if (clauseSatisfied(clause))
                continue;

            for (int lit : clause) {

                int var = std::abs(lit);

                if (assignment[var] != -1)
                    continue;

                if (lit > 0) {

                    if (polarity[var] == 0)
                        polarity[var] = 1;
                    else if (polarity[var] == -1)
                        polarity[var] = 2;
                }
                else {

                    if (polarity[var] == 0)
                        polarity[var] = -1;
                    else if (polarity[var] == 1)
                        polarity[var] = 2;
                }
            }
        }

        for (int i = 1; i <= numVars; i++) {

            if (assignment[i] != -1)
                continue;

            if (polarity[i] == 1)
                assignment[i] = 1;

            if (polarity[i] == -1)
                assignment[i] = 0;
        }
    }

    int chooseVariable() {

        for (int i = 1; i <= numVars; i++) {

            if (assignment[i] == -1)
                return i;
        }

        return -1;
    }

    bool solveRecursive() {

        if (!unitPropagation())
            return false;

        pureLiteralElimination();

        if (hasConflict())
            return false;

        if (allSatisfied())
            return true;

        int var = chooseVariable();

        if (var == -1)
            return false;

        std::vector<int> backup = assignment;

        assignment[var] = 1;

        if (solveRecursive())
            return true;

        assignment = backup;

        assignment[var] = 0;

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
