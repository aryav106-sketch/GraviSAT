#ifndef GRAVISAT_WATCHER_H
#define GRAVISAT_WATCHER_H

#include <vector>

class Watcher {

public:

    Watcher();

    void addWatch(int literal);

    bool propagate();

private:

    std::vector<int> watchedLiterals;
};

#endif
