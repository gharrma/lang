#ifndef PARSE_PARSE_H
#define PARSE_PARSE_H
#include <iostream>
#include "ast.h"
#include "lex.h"

using std::unique_ptr;

namespace parse {

class Parser {
public:
    Parser(Lexer& lex): lex_(lex) {}
    unique_ptr<Expr> ParseExpr();

private:
    unique_ptr<Expr> ParsePrimaryExpr();
    unique_ptr<Expr> ParseSecondaryExpr(unique_ptr<Expr> lhs, int32_t min_prec);
    [[noreturn]] void Expected(std::string expectation = "");

    Lexer& lex_;
};

} // namespace parse

#endif // PARSE_PARSE_H
