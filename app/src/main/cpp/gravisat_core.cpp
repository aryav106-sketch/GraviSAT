#include "solver.h"

GraviSATCore::GraviSATCore() {
    assignment.clear();
}

bool GraviSATCore::solve() {
    assignment.push_back(1);
    return true;
}

std::string GraviSATCore::getResult() {
    if (assignment.empty()) {
        return "UNSAT";
    }

    return "SAT";
}
