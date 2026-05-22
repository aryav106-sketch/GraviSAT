#ifndef GRAVISAT_DIMACS_PARSER_H
#define GRAVISAT_DIMACS_PARSER_H

#include <string>

class DimacsParser {

public:

    static bool validate(const std::string& cnf);
};

#endif
