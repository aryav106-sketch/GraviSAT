#include "gravisat_core.h"
#include "solver_state.h"

#include <sstream>
#include <string>
#include <vector>

extern void initializeWatchers(
    SolverState& s);

extern Clause* propagate(
    SolverState& s);

extern std::vector<int>
analyzeConflict(
    SolverState& s,
    Clause* conflictClause,
    int& outBacktrackLevel);

extern void backtrack(
    SolverState& s,
    int targetLevel);

extern int addLearnedClause(
    SolverState& s,
    const std::vector<int>& lits);

extern int pickBranchLiteral(
    SolverState& s);

extern bool allAssigned(
    SolverState& s);

extern void initializeVariableActivities(
    SolverState& s);

extern bool shouldRestart(
    SolverState& s);

extern void performRestart(
    SolverState& s);

extern void reduceLearnedClauses(
    SolverState& s);

extern void decayActivities(
    SolverState& s);

extern void decayClauseActivities(
    SolverState& s);

static bool enqueueLiteral(
    SolverState& s,
    int lit,
    int reason) {

    int v =
        abs(lit);

    int assign =
        lit > 0 ? 1 : 0;

    if (v <= 0 ||
        v >= s.vars.size())
        return false;

    if (s.vars[v]
            .value != -1) {

        return s.vars[v]
            .value == assign;
    }

    s.vars[v]
        .value = assign;

    s.vars[v]
        .level =
            s.currentLevel;

    s.vars[v]
        .reason =
            reason;

    s.trail.push_back({
        lit,
        s.currentLevel,
        reason
    });

    s.propagationQueue
        .push(lit);

    return true;
}

static bool parseDIMACS(
    const std::string& cnf,
    SolverState& s) {

    std::stringstream ss(cnf);

    std::string line;

    int vars = 0;

    while (
        std::getline(
            ss,
            line)) {

        if (line.empty())
            continue;

        if (line[0] == 'c')
            continue;

        if (line[0] == 'p') {

            std::stringstream ls(line);

            std::string tmp;

            int clauses;

            ls >> tmp;
            ls >> tmp;
            ls >> vars;
            ls >> clauses;

            s.vars.resize(
                vars + 1);

            continue;
        }

        std::stringstream ls(line);

        std::vector<int>
            clause;

        int lit;

        while (ls >> lit) {

            if (lit == 0)
                break;

            clause.push_back(lit);
        }

        if (!clause.empty()) {

            s.clauses.push_back(
                Clause(
                    clause,
                    false
                )
            );
        }
    }

    return !s.clauses.empty();
}

static bool solveInternal(
    SolverState& s) {

    initializeWatchers(s);

    initializeVariableActivities(s);

    for (int i = 0;
         i < s.clauses.size();
         i++) {

        Clause& c =
            s.clauses[i];

        if (c.lits.size() == 1) {

            if (!enqueueLiteral(
                    s,
                    c.lits[0],
                    i)) {

                return false;
            }
        }
    }

    Clause* conflict =
        propagate(s);

    if (conflict != nullptr)
        return false;

    while (true) {

        if (allAssigned(s)) {

            return true;
        }

        if (shouldRestart(s)) {

            performRestart(s);
        }

        int decisionLit =
            pickBranchLiteral(s);

        if (decisionLit == 0) {

            return true;
        }

        s.currentLevel++;

        if (!enqueueLiteral(
                s,
                decisionLit,
                -1)) {

            return false;
        }

        while (true) {

            Clause* confl =
                propagate(s);

            if (confl == nullptr)
                break;

            s.conflicts++;

            if (s.currentLevel == 0) {

                return false;
            }

            int backtrackLevel =
                0;

            std::vector<int>
                learned =
                    analyzeConflict(
                        s,
                        confl,
                        backtrackLevel);

            if (learned.empty()) {

                return false;
            }

            backtrack(
                s,
                backtrackLevel);

            int learnedIdx =
                addLearnedClause(
                    s,
                    learned);

            enqueueLiteral(
                s,
                learned[0],
                learnedIdx);

            decayActivities(s);

            decayClauseActivities(s);

            if (
                s.conflicts %
                200 == 0) {

                reduceLearnedClauses(
                    s);
            }
        }
    }
}

static std::string buildResult(
    SolverState& s,
    bool sat) {

    std::stringstream out;

    if (!sat) {

        out <<
            "UNSATISFIABLE\n";

        return out.str();
    }

    out <<
        "SATISFIABLE\n\n";

    for (int v = 1;
         v < s.vars.size();
         v++) {

        out
            << "x"
            << v
            << " = ";

        if (s.vars[v]
                .value == 1) {

            out <<
                "TRUE";
        }
        else if (
            s.vars[v]
                .value == 0) {

            out <<
                "FALSE";
        }
        else {

            out <<
                "UNASSIGNED";
        }

        out << "\n";
    }

    out << "\n";

    out <<
        "Conflicts: "
        << s.conflicts
        << "\n";

    out <<
        "Propagations: "
        << s.propagations
        << "\n";

    out <<
        "Decisions: "
        << s.decisions
        << "\n";

    out <<
        "Restarts: "
        << s.restarts
        << "\n";

    return out.str();
}

std::string solveCNF(
    const std::string& cnfText) {

    SolverState s;

    bool ok =
        parseDIMACS(
            cnfText,
            s);

    if (!ok) {

        return
            "INVALID DIMACS INPUT";
    }

    bool sat =
        solveInternal(s);

    return buildResult(
        s,
        sat);
}
