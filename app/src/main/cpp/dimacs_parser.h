#ifndef DIMACS_PARSER_H
#define DIMACS_PARSER_H

#include <string>

class DimacsParser {

public:

    bool parse(const std::string& cnf);
};

#endif
