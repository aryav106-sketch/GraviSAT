#include "gravisat_core.h"

GraviSATCore::GraviSATCore() {

    state.conflicts = 0;

    state.decisions = 0;
}

std::string GraviSATCore::getResult() {

    return "GraviSAT Native Engine Running";
}
