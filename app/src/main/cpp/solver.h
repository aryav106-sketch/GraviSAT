#ifndef SOLVER_H
#define SOLVER_H

#include <string>
#include <vector>

class GraviSATCore {
public:
    GraviSATCore();

    std::string getResult();

    bool solve();

private:
    std::vector<int> assignment;
};

#endif
