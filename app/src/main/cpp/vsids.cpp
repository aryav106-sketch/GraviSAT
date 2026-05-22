#include "vsids.h"

VSIDS::VSIDS() {
}

void VSIDS::bumpActivity(int variable) {

    activity[variable] += 1.0;
}

int VSIDS::selectVariable() {

    int bestVariable = -1;

    double bestScore = -1.0;

    for (const auto& pair : activity) {

        if (pair.second > bestScore) {

            bestScore = pair.second;

            bestVariable = pair.first;
        }
    }

    return bestVariable;
}
