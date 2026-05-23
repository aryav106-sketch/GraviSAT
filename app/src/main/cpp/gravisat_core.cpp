#include <jni.h>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <algorithm>

struct Decision {

    int variable;
    int value;
    int level;
};

class GraviSAT {

private:

    int numVars;

    int conflicts;

    int decisionsCount;

    std::vector<std::vector<int>> clauses;

    std::vector<std::vector<int>> learnedClauses;

    std::vector<int> assignment;

    std::vector<double> activity;

    std::vector<Decision> trail;

public:

    GraviSAT() {

        numVars = 0;

        conflicts = 0;

        decisionsCount = 0;
    }

    bool parseDIMACS(const std::string &input) {

        clauses.clear();

        learnedClauses.clear();

        assignment.clear();

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

    bool processClauseSet(
            std::vector<std::vector<int>>& db) {

        for (auto &clause : db) {

            bool satisfied = false;

            bool undecided = false;

            int unassigned = 0;

            int lastLit = 0;

            for (int lit : clause) {

                int var = std::abs(lit);

                int val = assignment[var];

                if (val == -1) {

                    undecided = true;

                    unassigned++;

                    lastLit = lit;
                }

                if (literalTrue(lit)) {

                    satisfied = true;

                    break;
                }
            }

            if (!satisfied && !undecided) {

                conflicts++;

                learnConflictClause(clause);

                return false;
            }

            if (!satisfied && unassigned == 1) {

                int var = std::abs(lastLit);

                int value = (lastLit > 0) ? 1 : 0;

                assignment[var] = value;
            }
        }

        return true;
    }

    void learnConflictClause(
            const std::vector<int>& conflictClause) {

        std::vector<int> learned;

        for (int lit : conflictClause) {

            learned.push_back(-lit);

            activity[std::abs(lit)] += 5.0;
        }

        learnedClauses.push_back(learned);
    }

    bool propagate() {

        bool changed = true;

        while (changed) {

            changed = false;

            size_t before =
                    learnedClauses.size();

            if (!processClauseSet(clauses))
                return false;

            if (!processClauseSet(learnedClauses))
                return false;

            if (learnedClauses.size() != before)
                changed = true;
        }

        return true;
    }

    bool allSatisfied() {

        for (auto &clause : clauses) {

            if (!clauseSatisfied(clause))
                return false;
        }

        for (auto &clause : learnedClauses) {

            if (!clauseSatisfied(clause))
                return false;
        }

        return true;
    }

    int chooseVariable() {

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

    bool solveRecursive(int level) {

        if (!propagate())
            return false;

        if (allSatisfied())
            return true;

        int var = chooseVariable();

        if (var == -1)
            return false;

        decisionsCount++;

        {
            auto backup = assignment;

            assignment[var] = 1;

            trail.push_back({var, 1, level});

            decayActivities();

            if (solveRecursive(level + 1))
                return true;

            assignment = backup;

            trail.pop_back();
        }

        {
            auto backup = assignment;

            assignment[var] = 0;

            trail.push_back({var, 0, level});

            decayActivities();

            if (solveRecursive(level + 1))
                return true;

            assignment = backup;

            trail.pop_back();
        }

        return false;
    }

    std::string solveSAT() {

        bool sat = solveRecursive(0);

        std::stringstream out;

        if (!sat) {

            out << "UNSATISFIABLE\n\n";
        }
        else {

            out << "SATISFIABLE\n\n";

            for (int i = 1; i <= numVars; i++) {

                out << "x" << i << " = ";

                if (assignment[i] == 1)
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
