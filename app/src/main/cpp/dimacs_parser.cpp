#include "dimacs_parser.h"

bool DimacsParser::parse(const std::string& cnf) {

    if (cnf.empty()) {
        return false;
    }

    if (cnf.find("p cnf") == std::string::npos) {
        return false;
    }

    return true;
}
