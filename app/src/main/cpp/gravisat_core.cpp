#include "gravisat_core.h"

GraviSATCore::GraviSATCore() {

    reset();
}

void GraviSATCore::reset() {

    state.vars = 0;
    state.clauses = 0;
    state.currentLevel = 0;

    state.assignment.clear();
    state.trail.clear();
    state.decision_level.clear();
    state.vars_data.clear();
}

bool GraviSATCore::solve() {

    return true;
}

std::string GraviSATCore::getResult() {

    if (solve()) {
        return "SATISFIABLE";
    }

    return "UNSATISFIABLE";
}
