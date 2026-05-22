#include "watcher.h"

Watcher::Watcher() {
}

void Watcher::addWatch(int literal) {

    watchedLiterals.push_back(literal);
}

bool Watcher::propagate() {

    return true;
}
