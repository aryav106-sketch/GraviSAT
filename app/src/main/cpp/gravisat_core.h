#ifndef GRAVISAT_CORE_H
#define GRAVISAT_CORE_H

#include <string>
#include "solver_state.h"

class GraviSATCore {

public:

    SolverState state;

    GraviSATCore();

    bool solve();

    void reset();

    std::string getResult();
};

#endif
