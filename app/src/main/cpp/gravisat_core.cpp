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

    bool isSatisfied() {
        for (auto &clause : clauses) {

            bool sat = false;

            for (int lit : clause) {

                int var = std::abs(lit);

                if (var >= assignment.size())
                    continue;

                int val = assignment[var];

                if (val == -1)
                    continue;

                if ((lit > 0 && val == 1) ||
                    (lit < 0 && val == 0)) {

                    sat = true;
                    break;
                }
            }

            if (!sat)
                return false;
        }

        return true;
    }

    bool hasConflict() {
        for (auto &clause : clauses) {

            bool sat = false;
            bool undecided = false;

            for (int lit : clause) {

                int var = std::abs(lit);

                int val = assignment[var];

                if (val == -1) {
                    undecided = true;
                }

                if ((lit > 0 && val == 1) ||
                    (lit < 0 && val == 0)) {

                    sat = true;
                    break;
                }
            }

            if (!sat && !undecided)
                return true;
        }

        return false;
    }

    bool dpll(int var) {

        if (hasConflict())
            return false;

        if (var > numVars)
            return isSatisfied();

        if (assignment[var] != -1)
            return dpll(var + 1);

        assignment[var] = 1;

        if (dpll(var + 1))
            return true;

        assignment[var] = 0;

        if (dpll(var + 1))
            return true;

        assignment[var] = -1;

        return false;
    }

    std::string solveSAT() {

        bool result = dpll(1);

        if (!result)
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
        jobject /* this */,
        jstring inputText) {

    const char *raw =
            env->GetStringUTFChars(inputText, 0);

    std::string dimacs(raw);

    env->ReleaseStringUTFChars(inputText, raw);

    GraviSAT solver;

    solver.parseDIMACS(dimacs);

    std::string result = solver.solveSAT();

    return env->NewStringUTF(result.c_str());
}
