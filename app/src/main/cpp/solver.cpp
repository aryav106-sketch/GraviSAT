#include "solver.h"
#include "dimacs_parser.h"
#include "watcher.h"
#include "vsids.h"
#include "analyzer.h"

Solver::Solver()
        : variables(0),
          clauses(0) {
}

bool Solver::parseCNF(const std::string& cnf) {

    return DimacsParser::validate(cnf);
}

bool Solver::dpll() {

    Watcher watcher;

    watcher.addWatch(1);

    bool propagation = watcher.propagate();

    if (!propagation) {

        Analyzer analyzer;

        return analyzer.analyzeConflict();
    }

    VSIDS heuristic;

    heuristic.bumpActivity(1);

    int variable = heuristic.selectVariable();

    if (variable == -1) {

        return false;
    }

    return true;
}

bool Solver::solve(const std::string& cnf) {

    bool parsed = parseCNF(cnf);

    if (!parsed) {

        return false;
    }

    return dpll();
}
