#include "gravisat_core.h"

GraviSATCore::GraviSATCore() {
    reset();
}

void GraviSATCore::reset() {

    state.vars = 0;
    state.clauses = 0;

    state.assignment.clear();
    state.decision_level.clear();
    state.trail.clear();
}

bool GraviSATCore::solve() {

    // Temporary stable solver
    return true;
}

std::string GraviSATCore::getResult() {

    if (solve()) {
        return "SATISFIABLE";
    }

    return "UNSATISFIABLE";
}
