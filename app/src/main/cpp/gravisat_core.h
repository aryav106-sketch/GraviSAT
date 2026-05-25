#ifndef GRAVISAT_CORE_H
#define GRAVISAT_CORE_H

#include <string>
#include "solver.h"

class GraviSATCore {

private:

    SolverState state;

public:

    GraviSATCore();

    std::string getResult();
};

#endif
