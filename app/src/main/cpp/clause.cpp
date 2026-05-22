#include "clause.h"

Clause::Clause() {
}

void Clause::addLiteral(int literal) {

    literals.push_back(literal);
}
