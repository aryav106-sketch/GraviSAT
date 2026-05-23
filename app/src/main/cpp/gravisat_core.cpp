#include <jni.h>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <chrono>

class GraviSAT {
private:
    int vars;
    int clausesCount;

    std::vector<std::vector<int>> clauses;
    std::vector<int> assignment;

    long long decisions = 0;
    long long propagations = 0;

public:
    GraviSAT(int v) {
        vars = v;
        assignment.resize(vars + 1, -1);
    }

    void addClause(const std::vector<int>& clause) {
        clauses.push_back(clause);
    }

    bool unitPropagation() {
        bool changed = true;

        while (changed) {
            changed = false;

            for (const auto& clause : clauses) {

                int unassigned = 0;
                int lastLit = 0;
                bool satisfied = false;

                for (int lit : clause) {
                    int var = std::abs(lit);

                    if (assignment[var] == -1) {
                        unassigned++;
                        lastLit = lit;
                    }
                    else {
                        bool val = assignment[var];

                        if ((lit > 0 && val) || (lit < 0 && !val)) {
                            satisfied = true;
                            break;
                        }
                    }
                }

                if (satisfied)
                    continue;

                if (unassigned == 0)
                    return false;

                if (unassigned == 1) {
                    int var = std::abs(lastLit);

                    assignment[var] = (lastLit > 0) ? 1 : 0;

                    propagations++;
                    changed = true;
                }
            }
        }

        return true;
    }

    bool allSatisfied() {
        for (const auto& clause : clauses) {

            bool satisfied = false;

            for (int lit : clause) {

                int var = std::abs(lit);

                if (assignment[var] == -1)
                    continue;

                bool val = assignment[var];

                if ((lit > 0 && val) || (lit < 0 && !val)) {
                    satisfied = true;
                    break;
                }
            }

            if (!satisfied)
                return false;
        }

        return true;
    }

    int chooseVariable() {
        for (int i = 1; i <= vars; i++) {
            if (assignment[i] == -1)
                return i;
        }

        return -1;
    }

    bool dpll() {

        if (!unitPropagation())
            return false;

        if (allSatisfied())
            return true;

        int var = chooseVariable();

        if (var == -1)
            return false;

        decisions++;

        {
            auto backup = assignment;

            assignment[var] = 1;

            if (dpll())
                return true;

            assignment = backup;
        }

        {
            auto backup = assignment;

            assignment[var] = 0;

            if (dpll())
                return true;

            assignment = backup;
        }

        return false;
    }

    std::string solve() {

        auto start = std::chrono::high_resolution_clock::now();

        bool sat = dpll();

        auto end = std::chrono::high_resolution_clock::now();

        double timeMs =
            std::chrono::duration<double, std::milli>(end - start).count();

        std::stringstream ss;

        if (sat) {
            ss << "SATISFIABLE\n\n";

            for (int i = 1; i <= vars; i++) {

                if (assignment[i] == 1)
                    ss << "x" << i << " = TRUE\n";

                else if (assignment[i] == 0)
                    ss << "x" << i << " = FALSE\n";

                else
                    ss << "x" << i << " = UNASSIGNED\n";
            }
        }
        else {
            ss << "UNSATISFIABLE\n";
        }

        ss << "\n";
        ss << "Decisions: " << decisions << "\n";
        ss << "Propagations: " << propagations << "\n";
        ss << "Time: " << timeMs << " ms\n";

        return ss.str();
    }
};

extern "C"
JNIEXPORT jstring JNICALL
Java_com_gravisat_shield_MainActivity_solveSAT(
        JNIEnv* env,
        jobject,
        jstring input) {

    const char* raw = env->GetStringUTFChars(input, 0);

    std::string cnf(raw);

    env->ReleaseStringUTFChars(input, raw);

    std::stringstream ss(cnf);

    std::string line;

    int vars = 0;
    int clauses = 0;

    GraviSAT* solver = nullptr;

    while (std::getline(ss, line)) {

        if (line.empty())
            continue;

        if (line[0] == 'c')
            continue;

        if (line[0] == 'p') {

            std::stringstream header(line);

            std::string tmp;

            header >> tmp;
            header >> tmp;
            header >> vars;
            header >> clauses;

            solver = new GraviSAT(vars);

            continue;
        }

        if (!solver)
            continue;

        std::stringstream clauseStream(line);

        int lit;

        std::vector<int> clause;

        while (clauseStream >> lit) {

            if (lit == 0)
                break;

            clause.push_back(lit);
        }

        if (!clause.empty())
            solver->addClause(clause);
    }

    if (!solver) {
        return env->NewStringUTF("Invalid DIMACS CNF");
    }

    std::string result = solver->solve();

    delete solver;

    return env->NewStringUTF(result.c_str());
}
