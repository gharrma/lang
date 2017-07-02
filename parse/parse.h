#ifndef PARSE_H
#define PARSE_H
#include <iostream>
#include "lex.h"

namespace parse {

class Parser {
public:
    Parser(std::istream& is): lex_(is) {}

private:
    Lexer lex_;
};

} // namespace parse

#endif // PARSE_H
