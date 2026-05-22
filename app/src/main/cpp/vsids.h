#ifndef GRAVISAT_VSIDS_H
#define GRAVISAT_VSIDS_H

#include <unordered_map>

class VSIDS {

public:

    VSIDS();

    void bumpActivity(int variable);

    int selectVariable();

private:

    std::unordered_map<int, double> activity;
};

#endif
