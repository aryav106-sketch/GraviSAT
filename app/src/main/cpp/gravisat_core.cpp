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

struct Clause {

    std::vector<int> lits;

    int watch1;

    int watch2;
};

class GraviSAT {

private:

    int numVars;

    int conflicts;

    int decisions;

    int propagations;

    int currentLevel;

    std::vector<Clause> clauses;

    std::vector<Clause> learnedClauses;

    std::vector<VariableState> vars;

    std::vector<double> activity;

    std::queue<int> propagationQueue;

public:

    GraviSAT() {

        numVars = 0;

        conflicts = 0;

        decisions = 0;

        propagations = 0;

        currentLevel = 0;
    }

    Clause makeClause(
            const std::vector<int>& lits) {

        Clause c;

        c.lits = lits;

        c.watch1 = 0;

        if (lits.size() > 1)
            c.watch2 = 1;
        else
            c.watch2 = 0;

        return c;
    }

    bool parseDIMACS(const std::string &input) {

        clauses.clear();

        learnedClauses.clear();

        vars.clear();

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

                std::vector<int> lits;

                while (ls >> lit) {

                    if (lit == 0)
                        break;

                    lits.push_back(lit);

                    activity[std::abs(lit)] += 1.0;
                }

                if (!lits.empty()) {

                    clauses.push_back(
                            makeClause(lits));
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

        propagationQueue.push(var);

        propagations++;

        return true;
    }

    bool propagateClauseSet(
            std::vector<Clause>& db) {

        for (size_t c = 0; c < db.size(); c++) {

            Clause &clause = db[c];

            int lit1 =
                    clause.lits[clause.watch1];

            int lit2 =
                    clause.lits[clause.watch2];

            if (literalTrue(lit1) ||
                literalTrue(lit2))
                continue;

            bool movedWatch = false;

            for (size_t i = 0;
                 i < clause.lits.size();
                 i++) {

                if ((int)i == clause.watch1 ||
                    (int)i == clause.watch2)
                    continue;

                int lit = clause.lits[i];

                if (!literalFalse(lit)) {

                    if (literalFalse(lit1))
                        clause.watch1 = i;
                    else
                        clause.watch2 = i;

                    movedWatch = true;

                    break;
                }
            }

            if (movedWatch)
                continue;

            if (literalFalse(lit1) &&
                literalFalse(lit2)) {

                conflicts++;

                learnClause(clause.lits);

                return false;
            }

            if (!literalFalse(lit1)) {

                if (!assignLiteral(
                        lit1,
                        currentLevel,
                        (int)c)) {

                    conflicts++;

                    return false;
                }
            }
            else {

                if (!assignLiteral(
                        lit2,
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

            if (!propagateClauseSet(clauses))
                return false;

            if (!propagateClauseSet(
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

        learnedClauses.push_back(
                makeClause(learned));
    }

    bool allSatisfied() {

        for (auto &clause : clauses) {

            bool sat = false;

            for (int lit : clause.lits) {

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

    void resetAssignments() {

        for (int i = 1; i <= numVars; i++) {

            vars[i].value = -1;

            vars[i].level = -1;

            vars[i].reason = -1;
        }

        while (!propagationQueue.empty())
            propagationQueue.pop();
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
            auto backup = vars;

            assignLiteral(
                    var,
                    currentLevel,
                    -1);

            decayActivities();

            if (solveRecursive())
                return true;

            vars = backup;
        }

        {
            auto backup = vars;

            assignLiteral(
                    -var,
                    currentLevel,
                    -1);

            decayActivities();

            if (solveRecursive())
                return true;

            vars = backup;
        }

        currentLevel--;

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
